#pragma once
#include "math.h"
#include "Framebuffer.h"
#include "simpleFonts.h" 
#include <stdint.h>

class NewBasicRenderer{
    private:
    //rendering data
    Framebuffer* TargetFramebuffer;
	char* TargetCharBuffer;
    PSF1_FONT* PSF1_Font;

    //buffers
    uint32_t* BackgroundBuffer;
    uint32_t* TextBackgroundBuffer;
    uint32_t* TextForgroundBuffer;
    uint32_t* ForgroundBuffer;

    //default colors
    unsigned int DefaultTextColor;
    unsigned int DefaultBackgroundColor;
    
    //internal things
    uint32_t* FetchBuffer(uint32_t buffer);
    void ReEvaluateFrameBufferPix(uint32_t x, uint32_t y);
    void ClearBuffer(bool updateScreen, uint32_t color, uint32_t bufferID);
    void UpdateScreen();

    public:
    NewBasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font);

    //rendering
    void DrawPix(uint32_t x, uint32_t y, uint32_t colour);
    uint32_t GetPix(uint32_t x, uint32_t y);
    uint32_t GetPix(uint32_t x, uint32_t y, uint32_t bufferID);

    void DrawPix(uint32_t x, uint32_t y, uint32_t colour, uint32_t bufferID){
        DrawPix(x,y,colour, bufferID, true);
    }
    void DrawPix(uint32_t x, uint32_t y, uint32_t colour, uint32_t bufferID, bool updateScreen);


    inline void ClearBuffer(uint32_t bufferID){
        ClearBuffer(DefaultBackgroundColor, bufferID);
    }
    
    void ClearBuffer(uint32_t color, uint32_t bufferID){
        ClearBuffer(true, color, bufferID);
    }

    inline void ClearScreen(){
        ClearScreen(DefaultBackgroundColor);
    }
    
    void ClearScreen(uint32_t color);

    //text
    inline void DrawChar(char chr, unsigned int xOff, unsigned int yOff){
        DrawChar(chr, xOff, yOff, DefaultTextColor, DefaultBackgroundColor);
    }
    inline void DrawChar(char chr, unsigned int xOff, unsigned int yOff, uint32_t fColor){
        DrawChar(chr, xOff, yOff, fColor, DefaultBackgroundColor);
    }

    void DrawChar(char chr, unsigned int xOff, unsigned int yOff, uint32_t fColor, uint32_t bColor);

    inline uint32_t GetWidth(){
        return TargetFramebuffer->Width;
    }
    inline uint32_t GetHeight(){
        return TargetFramebuffer->Height;
    }
    inline uint32_t GetCharsX(){
        return GetWidth() / GetCharWidth();
    }
    inline uint32_t GetCharsY(){
        return GetHeight() / GetCharWidth();
    }

    inline uint32_t GetCharWidth(){
        return 8;
    }
    inline uint32_t GetCharHeight(){
        return 16;
    }

    void ScrollScreenY(int offset); //positive scrolls upword
};

extern NewBasicRenderer* NewGlobalRenderer;