import numpy as np
import torch
import torch.nn as nn
import torch.nn.functional as F
from hausdorff import hausdorff_distance

class CE_func(nn.Module):
    def __init__(self):
        super().__init__()
        self.func = nn.CrossEntropyLoss()
        self.reset()

    def forward(self, preds, targets):
        loss = self.func(preds, targets)
        self.sum += loss
        self.count += 1
        return loss

    def reset(self):
        self.count = 0
        self.sum = 0

    def compute(self):
        loss = self.sum / self.count
        self.reset()
        return loss
    
    
class IOU(nn.Module):
    '''
    Calculate Intersection over Union (IoU) for semantic segmentation.
    
    Args:
        logits (torch.Tensor): Predicted tensor of shape (batch_size, num_classes, height, width, (depth))
        targets (torch.Tensor): Ground truth tensor of shape (batch_size, height, width, (depth))
        num_classes (int): Number of classes

    Returns:
        tensor: Mean Intersection over Union (IoU) for the batch.
        list: List of IOU score for each class
    '''
    def __init__(self, num_classes, ignore_index=[0]):
        super(IOU, self).__init__()
        self.num_classes = num_classes
        self.ignore_index = ignore_index
        
    def forward(self, logits, targets):
        pred = logits.argmax(dim=1)        
        targets = targets.argmax(dim=1)       
        cls_iou = []
        for cls in range(self.num_classes):
            if cls in self.ignore_index: 
                continue
            pred_mask = (pred == cls)
            targets_mask = (targets == cls)
                            
            intersection = (pred_mask & targets_mask).sum().float()
            union = (pred_mask | targets_mask).sum().float()
            
            if union == 0: 
                iou = 1.0 
            else: 
                iou = (intersection / union).item()
                
            cls_iou.append(iou)
        
        mean_iou = torch.tensor(sum(cls_iou) / (self.num_classes - len(self.ignore_index)))
        return mean_iou, 1-mean_iou, cls_iou


class BinaryDice(nn.Module):
    '''
    Calculate Binary Dice score and Dice loss for binary segmentation or each class in Multiclass segmentation
    
    Args:
        logits (torch.Tensor): Predicted tensor of shape (batch_size, height, width, (depth))
        targets (torch.Tensor): Ground truth tensor of shape (batch_size, height, width. (depth))
        
    Returns:
        tensor: Dice score
        tensor: Dice loss
    '''
    def __init__(self, smooth=1e-5, p=2):
        super(BinaryDice, self).__init__()
        self.smooth = smooth
        self.p = p

    def forward(self, logits, targets):
        assert logits.shape[0] == targets.shape[0], "logits & targets batch size don't match"
        smooth = 1e-5
        intersect = torch.sum(logits * targets)        
        y_sum = torch.sum(targets * targets)
        z_sum = torch.sum(logits * logits)
        dice = (2 * intersect + smooth) / (z_sum + y_sum + smooth)
        loss = 1 - dice
        return dice, loss
        

class Dice(nn.Module):
    '''
    Calculate Dice score and Dice loss for multiclass semantic segmentation
    
    Args:
        output (torch.Tensor): Predicted tensor of shape (batch_size, num_classes, height, width, (depth))
        targets (torch.Tensor): Ground truth tensor of shape (batch_size, height, width, (depth))
        num_classes (int): Number of classes 
        
    Returns:
        tensor: Mean dice score over classes
        tensor: Mean dice loss over classes
        list: dice score for each classes
        listL dice loss for each classes
    '''
    def __init__(self, num_classes, weight, softmax=True, ignore_index=[0]):
        super(Dice, self).__init__()
        self.num_classes = num_classes
        self.weight = weight
        self.softmax = softmax
        self.ignore_index = ignore_index
        self.binary_dice = BinaryDice()

    def forward(self, logits, targets):
        assert logits.shape == targets.shape, 'logits & targets shape do not match'
        if self.softmax: 
            logits = F.softmax(logits, dim=1)
        
        DICE, LOSS = 0.0, 0.0
        CLS_DICE = []
        for clx in range(targets.shape[1]):
            if clx in self.ignore_index: 
                continue
            dice, loss = self.binary_dice(logits[:, clx], targets[:, clx])
            CLS_DICE.append(dice.item())
            if self.weight is not None: 
                loss *= self.weight[clx]
            DICE += dice
            LOSS += loss

        num_valid_classes = self.num_classes - len(self.ignore_index)
        return DICE / num_valid_classes, LOSS / num_valid_classes, CLS_DICE
    

    
class Dice_func(nn.Module):
    def __init__(self, num_classes, weight):
        super().__init__()
        self.func = Dice(num_classes, weight)
        self.cls_dice_list = []
        self.reset()
        
    def forward(self, logits, targets):
        self.count += 1
        dice, loss, cls_dice = self.func(logits, targets)
        self.sum += dice
        self.cls_dice_list.append(cls_dice)
        return dice, -torch.log(dice**2), cls_dice
    
    def reset(self):
        self.count = 0
        self.sum = 0
        
    def compute_cls(self):
        cls_dice_list = np.mean(np.array(self.cls_dice_list), axis=0)
        self.cls_dice_list = []
        return cls_dice_list.tolist()

    def compute(self):
        dice = self.sum / self.count
        self.reset()
        return dice
    

class IOU_func(nn.Module):
    def __init__(self, num_classes):
        super().__init__()
        self.func = IOU(num_classes)
        self.cls_iou_list = []
        self.reset()
        
    def forward(self, logits, targets):
        self.count += 1
        iou, loss, cls_iou = self.func(logits, targets)
        self.sum += iou
        self.cls_iou_list.append(cls_iou)
        return iou, -torch.log(iou**2), cls_iou
    
    def reset(self):
        self.count = 0
        self.sum = 0
        
    def compute_cls(self):
        cls_iou_list = np.mean(np.array(self.cls_iou_list), axis=0)
        self.cls_iout_list = []
        return cls_iou_list.tolist()

    def compute(self):
        iou = self.sum / self.count
        self.reset()
        return iou
    
    
class HD(nn.Module):
    def __init__(self):
        super().__init__()
        self.reset()
        
    def forward(self, logits, targets):
        _,logits = torch.max(logits, dim=1)
        _,targets = torch.max(targets, dim=1)
        
        logits = logits.detach().cpu().numpy()
        targets = targets.detach().cpu().numpy()
        
        hd = 0
        for index in range(logits.shape[0]):
            hd += hausdorff_distance(logits[index], targets[index], distance='euclidean')
        
        loss = hd / logits.shape[0]
        self.sum += loss
        self.count += 1
        return loss
    
    
    def compute(self):
        loss = self.sum / self.count
        self.reset()
        return loss
    
    def reset(self):
        self.sum = 0
        self.count = 0