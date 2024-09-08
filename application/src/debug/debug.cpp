/*
void CheckOpenGLError(const char *stmt, const char *fname, int line)
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error " << err << ", at " << fname << ":" << line << " - for " << stmt << std::endl;
        exit(1);
    }
}

#define GL_CHECK(stmt)                               \
    do                                               \
    {                                                \
        stmt;                                        \
        CheckOpenGLError(#stmt, __FILE__, __LINE__); \
    } while (0)

*/