#include "NewBasicRenderer.h"
#include "memory/heap.h"

NewBasicRenderer* NewGlobalRenderer;

NewBasicRenderer::NewBasicRenderer(Framebuffer* targetFramebuffer, PSF1_FONT* psf1_Font)
{
    TargetFramebuffer = targetFramebuffer;
    PSF1_Font = psf1_Font;
    DefaultTextColor = 0xffffffff;
    DefaultBackgroundColor = 0x00000000;
    
    BackgroundBuffer = new uint32_t[(targetFramebuffer->Height) * (targetFramebuffer->Width)];
    TextBackgroundBuffer = new uint32_t[(targetFramebuffer->Height) * (targetFramebuffer->Width)];
    TextForgroundBuffer = new uint32_t[(targetFramebuffer->Height) * (targetFramebuffer->Width)];
    ForgroundBuffer = new uint32_t[(targetFramebuffer->Height) * (targetFramebuffer->Width)];
    TargetCharBuffer = (char*)malloc(GetCharsX() * GetCharsY());
	
}

uint32_t* NewBasicRenderer::FetchBuffer(uint32_t buffer){
    switch(buffer){
        case 0:
        return ForgroundBuffer;
        case 1:
        return TextForgroundBuffer;
        case 2:
        return TextBackgroundBuffer;
        case 3:
        return BackgroundBuffer;
        default:
        return NULL;
    }
}

//epic CPU go burrr
void NewBasicRenderer::ReEvaluateFrameBufferPix(uint32_t x, uint32_t y){
    uint32_t color = 0;
    for(int i = 3; i >= 0; i --){
        uint32_t tempColor = this->GetPix(x,y,i);
        if((tempColor & 0xFF000000) != 0){
           color = tempColor;
        }
    }
    DrawPix(x,y, color);
}

inline void NewBasicRenderer::DrawPix(uint32_t x, uint32_t y, uint32_t colour, uint32_t bufferID, bool updateScreen){
    uint32_t* layerBuffer = FetchBuffer(bufferID);
    size_t index = x + y * this->GetWidth();
    layerBuffer[index] = colour;
    if(updateScreen){
        ReEvaluateFrameBufferPix(x,y);
    }
}


inline uint32_t NewBasicRenderer::GetPix(uint32_t x, uint32_t y, uint32_t bufferID){
    uint32_t* buffer = FetchBuffer(bufferID);
    size_t index = x + y * this->GetWidth();
    return buffer[index];
}

inline void NewBasicRenderer::DrawPix(uint32_t x, uint32_t y, uint32_t colour){
    *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4)) = colour;
}

inline uint32_t NewBasicRenderer::GetPix(uint32_t x, uint32_t y){
    return *(uint32_t*)((uint64_t)TargetFramebuffer->BaseAddress + (x*4) + (y * TargetFramebuffer->PixelsPerScanLine * 4));
}

void NewBasicRenderer::ClearScreen(uint32_t color){
    uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
    uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
    uint64_t fbHeight = TargetFramebuffer->Height;
    uint64_t fbSize = TargetFramebuffer->BufferSize;

    for (int verticalScanline = 0; verticalScanline < fbHeight; verticalScanline ++){
        uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
        for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++){
            *pixPtr = color;
        }
    }

    for(int i = 0; i < 4; i ++){
        ClearBuffer(false, color, i);
    }

    this->UpdateScreen();
}

void NewBasicRenderer::ScrollScreenY(int scroll){
    if(scroll == 0){
        return;
    }

    if(scroll > 0){
        
        if(false){
        for(int b = 0; b < 4; b ++){
            uint32_t* buffer = this->FetchBuffer(b);

            size_t offset = scroll * GetWidth();
            size_t index = GetWidth() * GetHeight() - 1 - offset;

            for(int i = index; i >= 0; i --){
                buffer[i + offset] = buffer[i]; 
            }
            for(int i = 0; i < scroll * GetWidth(); i ++){
                buffer[i] = DefaultBackgroundColor;
            }
        }
        }

        uint64_t fbBase = (uint64_t)TargetFramebuffer->BaseAddress;
        uint64_t bytesPerScanline = TargetFramebuffer->PixelsPerScanLine * 4;
        uint64_t fbHeight = TargetFramebuffer->Height;
        uint64_t fbSize = TargetFramebuffer->BufferSize;
        uint64_t offset = scroll * bytesPerScanline;

        for (int verticalScanline = fbHeight - scroll - 1; verticalScanline >= 0; verticalScanline --){
            uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
            for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++){
                *(uint32_t*)((uint64_t)pixPtr + offset) = *pixPtr;
            }
        }

        for (int verticalScanline = 0; verticalScanline < scroll; verticalScanline ++){
            uint64_t pixPtrBase = fbBase + (bytesPerScanline * verticalScanline);
            for (uint32_t* pixPtr = (uint32_t*)pixPtrBase; pixPtr < (uint32_t*)(pixPtrBase + bytesPerScanline); pixPtr ++){
                *pixPtr = DefaultBackgroundColor;
            }
        }
    }else{
        scroll = scroll * -1;

    }
}

void NewBasicRenderer::ClearBuffer(bool updateScreen, uint32_t color, uint32_t bufferID){
    uint32_t* buffer = FetchBuffer(bufferID);
    for(int i = 0; i < GetWidth() * GetHeight(); i ++){
        buffer[i] = color;
    }
    if(updateScreen){
        this->UpdateScreen();
    }
}

void NewBasicRenderer::UpdateScreen(){
    for(int x = 0; x < GetWidth(); x ++){
        for(int y = 0; y < GetHeight(); y ++){
            this->ReEvaluateFrameBufferPix(x,y);
        }
    }
}

void NewBasicRenderer::DrawChar(char chr, unsigned int xOff, unsigned int yOff, uint32_t fColor, uint32_t bColor)
{	
    unsigned int* pixPtr = (unsigned int*)TargetFramebuffer->BaseAddress;
    char* fontPtr = (char*)PSF1_Font->glyphBuffer + (chr * PSF1_Font->psf1_Header->charsize);
    for (unsigned long y = yOff; y < yOff + 16; y++){
        for (unsigned long x = xOff; x < xOff+8; x++){
            if ((*fontPtr & (0b10000000 >> (x - xOff))) > 0)
			{			
				TextForgroundBuffer[x + y * this->GetWidth()] = fColor;
                TextBackgroundBuffer[x + y * this->GetWidth()] = bColor;
            }else{
                TextForgroundBuffer[x + y * this->GetWidth()] = 0x0;
                TextBackgroundBuffer[x + y * this->GetWidth()] = bColor;
            }
            ReEvaluateFrameBufferPix(x,y);
        }
        fontPtr++;
    }
}