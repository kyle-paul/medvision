import torch
import sys
import os
from collections import OrderedDict
from tqdm import tqdm
import pandas as pd
from typing import Dict, List
from utils import write_csv, rlog
import torch.nn as nn
from metrics import CE_func, Dice_func, IOU_func, HD

class Trainer:
    def __init__(
        self,
        config: Dict = None,
        model: nn.Module = None,
        train_loader = None,
        val_loader = None,
        optimizer = None,
        classes: List = None,
        class_weight: List = None,
    ):
        
        self.config = config
        self.model = model
        self.train_loader = train_loader
        self.val_loader = val_loader
        self.optimizer = optimizer
        self.classes = classes
        self.grad_acc = config.grad_acc
        
        self.dice_func = Dice_func(config.num_classes, weight=class_weight)
        self.iou_func = IOU_func(config.num_classes)
        self.ce_func = CE_func()
        self.hd_func = HD()
        
        self.best_train_dice = 0.
        self.best_train_iou = 0.
        self.best_val_dice = 0.
        self.best_val_iou = 0.
        
        self.log = OrderedDict([
            ('epoch', []),                                          
            ('lr', []),                                             
            
            ('Train loss', []),                   
            ('Train dice score', []),                                     
            ('Train iou score', []),    
            ('Train ce loss', []),                                           
            ('Train hausdorff', []),                              
            
            ('Val loss', []), 
            ('Val dice score', []),                                          
            ('Val iou score', []),      
            ('Val ce loss', []),                                            
            ('Val hausdorff', []),                 
        ])
        
        self.iter_log_file = f'{config.log_dir}/iter_log.csv'
        self.iter_cls_dice = f'{config.log_dir}/iter_cls_dice.csv'
        self.iter_cls_iou =  f'{config.log_dir}/iter_cls_iou.csv'
        self.epo_log_file =  f'{config.log_dir}/epo_log.csv'
        self.epo_cls_dice = f'{config.log_dir}/epo_cls_dice.csv'
        self.epo_cls_iou =  f'{config.log_dir}/epo_cls_iou.csv'
        self.model_save_path = f'{config.log_dir}/{config.network}.pt'
        
        self.start_log()
        self.training_loop()
        
    def start_log(self):
        fieldnames = ['Total Loss', 'Dice Score', 'IoU Score', 'CE Loss', 'HausDorff Distance']
        assert self.config.num_classes == len(self.classes) + 1, "Mismatch num_classes"
        
        if not os.path.exists(self.iter_log_file): 
            write_csv(self.iter_log_file, fieldnames)    
        
        if not os.path.exists(self.iter_cls_dice): 
            write_csv(self.iter_cls_dice, self.classes)
            
        if not os.path.exists(self.iter_cls_iou): 
            write_csv(self.iter_cls_iou, self.classes)
            
        if not os.path.exists(self.epo_log_file): 
            write_csv(self.epo_log_file, fieldnames)    
        
        if not os.path.exists(self.epo_cls_dice): 
            write_csv(self.epo_cls_dice, self.classes)
            
        if not os.path.exists(self.epo_cls_iou): 
            write_csv(self.epo_cls_iou, self.classes)
            
        
        
    def logging(self, type, log):        
        print(f"{type} loss: {rlog(log['loss'])} - {type} dice score: {rlog(log['dice_score'])} - {type} iou Score: {rlog(log['iou_score'])} - {type} hausdorff: {rlog(log['hausdorff'])} - {type} ce loss: {rlog(log['ce_loss'])}")
            
    def training_loop(self):
        torch.compile(self.model)
        for epoch in range(self.config.epochs):
            print(f"Epoch: {epoch+1}/{self.config.epochs}")
            train_log = self.train_per_epoch()
            self.logging("Train", train_log)
            self.log['epoch'].append(epoch)
            self.log['lr'].append(self.optimizer.param_groups[0]['lr'])
            self.log['Train loss'].append(train_log['loss'])
            self.log['Train dice score'].append(train_log['dice_score'])
            self.log['Train iou score'].append(train_log['iou_score'])
            self.log['Train ce loss'].append(train_log['ce_loss'])
            self.log['Train hausdorff'].append(train_log['hausdorff']) 
            
            if self.config.val_mode: 
                val_log = self.val_per_epoch()
                self.logging("Val", val_log)
                self.log['Val loss'].append(val_log['loss'])
                self.log['Val dice score'].append(val_log['dice_score'])
                self.log['Val iou score'].append(val_log['iou_score'])
                self.log['Val ce loss'].append(val_log['ce_loss'])
                self.log['Val hausdorff'].append(val_log['hausdorff'])
            
            else:
                self.log['Val loss'].append(None)
                self.log['Val ce loss'].append(None)
                self.log['Val dice score'].append(None)
                self.log['Val dice loss'].append(None)
                self.log['Val iou score'].append(None)
                self.log['Val iou loss'].append(None)
                self.log['Val hausdorff'].append(None)
                
            pd.DataFrame(self.log).to_csv(self.epo_log_file, index=False)
            write_csv(self.epo_cls_dice, self.dice_func.compute_cls())
            write_csv(self.epo_cls_iou, self.iou_func.compute_cls())
            
            # Save best model
            if train_log['iou_score'] >= self.best_train_iou and train_log['dice_score'] >= self.best_train_dice \
                and val_log['iou_score'] >= self.best_val_iou and val_log['dice_score'] >= self.best_val_dice:
                    
                self.best_train_dice = train_log['dice_score']
                self.best_train_iou = train_log['iou_score']
                self.best_val_dice = val_log['dice_score']
                self.best_val_iou = val_log['iou_score']
                torch.save(self.model.state_dict(), self.model_save_path)
                
            if (epoch+1) % 1 == 0:
                print(f'BEST TRAIN DICE: {self.best_train_dice} - BEST TRAIN IOU: {self.best_train_iou} - BEST VAL DICE SCORE: {self.best_val_dice} - BEST VAL IOU: {self.best_val_iou}')
            
    
    def train_per_epoch(self):
        self.model.train()
        steps = len(self.train_loader)
        pbar = tqdm(steps)
        total_loss = 0.0
        
        for iter, (input, targets) in tqdm(enumerate(self.train_loader)):
            sys.stdout.write(f"\riter: {iter+1}/{steps}")
            sys.stdout.flush()
            
            input = input.cuda()
            targets = targets.cuda()
            logits = self.model(input)
            
            ce_loss = self.ce_func(logits, targets)
            dice_score, dice_loss, class_dice = self.dice_func(logits, targets)
            iou_score, iou_loss, class_iou = self.iou_func(logits, targets)
            hausdorff = self.hd_func(logits, targets)
            loss = dice_loss*0.4 + iou_loss*0.6
            total_loss += loss.item()
            
            write_csv(self.iter_log_file, [
                loss.item(),
                dice_score.item(),
                iou_score.item(),
                ce_loss.item(),
                hausdorff,
            ])
            
            write_csv(self.iter_cls_dice, class_dice)
            write_csv(self.iter_cls_iou, class_iou)
            
            loss.backward()
            if (iter + 1) % self.grad_acc == 0 or iter + 1 == steps:
                self.optimizer.step()
                self.optimizer.zero_grad()    
            pbar.update(1)
        
        pbar.close()
        return OrderedDict([
            ('loss', total_loss/steps),
            ('dice_score', self.dice_func.compute().item()),
            ('iou_score', self.iou_func.compute().item()),
            ('ce_loss', self.ce_func.compute().item()),
            ('hausdorff', self.hd_func.compute()),
        ])
        
    def val_per_epoch(self):
        self.model.eval()
        steps = len(self.val_loader)
        total_loss = 0.0
        
        with torch.inference_mode():
            for input, targets in self.val_loader:
                input = input.cuda()
                targets = targets.cuda()
                logits = self.model(input)
            
                _ = self.ce_func(logits, targets)
                _, dice_loss, _ = self.dice_func(logits, targets)
                _, iou_loss, _ = self.iou_func(logits, targets)
                _ = self.hd_func(logits, targets)
                loss = dice_loss*0.4 + iou_loss*0.6
                total_loss += loss.item()
                
        return OrderedDict([
            ('loss', total_loss/steps),
            ('dice_score', self.dice_func.compute().item()),
            ('iou_score', self.iou_func.compute().item()),
            ('ce_loss', self.ce_func.compute().item()),
            ('hausdorff', self.hd_func.compute()),
        ])