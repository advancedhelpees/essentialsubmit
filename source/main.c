#include <curl/curl.h> //curl
#include <3ds.h>       //libctru

#include <stdio.h>
#include "network.h"
#include "ui.h"
#include "serial.h"

DrawContext ctx;

#ifndef SERVER_ADDRESS
#define SERVER_ADDRESS "0.0.0.0"
#endif

void enter(char* inout) {
    SwkbdState swkbd;
    swkbdInit(&swkbd, SWKBD_TYPE_NORMAL, 1, 50);
    swkbdSetInitialText(&swkbd, inout);
    swkbdInputText(&swkbd, inout, 51);
    return;
}

int main() {
    C2D_SpriteSheet sheet = NULL;
    gfxInitDefault();
    romfsInit();
    if (!initSocket()) {
       goto fail;
    }

    int result;
    result = initUI();
    if (result) {
        printf("initUI returned %d", result);
        goto fail;
    }
    char discordtag[33] = "";
    char address[52] = SERVER_ADDRESS;
    initContext(&ctx);
    initColors(&ctx);
    int menustate = 0;
    char finaltext[200];
    bool usenandessential = false;
    setNANDEssentialSerial();
    setSDEssentialSerial();
    setTWLNSerial();
    setSecinfoSerial();


    sheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
    C2D_Sprite logo, soap;
    C2D_SpriteFromSheet(&logo, sheet, 0);
    C2D_SpriteFromSheet(&soap, sheet, 1);

    C2D_SpriteSetPos(&logo, 10, 10);
    C2D_SpriteSetScale(&logo, 0.4, 0.4);
    C2D_SpriteSetPos(&soap, SCREEN_WIDTH_BOTTOM/2, SCREEN_HEIGHT/2);
    C2D_SpriteSetScale(&soap, 0.7, 0.7);
    C2D_SpriteSetCenter(&logo, 0.0, 0.0);
    C2D_SpriteSetCenter(&soap, 0.5, 0.5);

    while (aptMainLoop()) {
        hidScanInput();
        touchPosition touch;
        hidTouchRead(&touch);
        u32 kDown = hidKeysDown();

        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(ctx.top, ctx.clrBgDark);
        C2D_TargetClear(ctx.bottom, ctx.clrBgDark);
        C2D_SceneBegin(ctx.bottom);
        C2D_DrawSprite(&soap);
        C2D_SceneBegin(ctx.top);
        drawText(115, 10, 0, 0.7, ctx.clrWhite, 0, "Bluehax\nessential.exefs Submitter");
        drawText(115, 55, 0, 0.4, ctx.clrWhite, 0, "made by gruetzig, modified by advancedhelpees");
        C2D_DrawSprite(&logo);

        drawText(160,  80, 0, 0.4, ctx.clrWhite, 0, "SD essential.exefs serial:");
        drawText(160,  90, 0, 0.4, ctx.clrWhite, 0, "NAND essential.exefs serial:");
        drawText(160, 100, 0, 0.4, ctx.clrWhite, 0, "SecureInfo serial:");
        drawText(160, 110, 0, 0.4, ctx.clrWhite, 0, "inspect.log serial:");
        drawText(320,  80, 0, 0.4, ctx.clrWhite, 0, "%s", getSDEssentialSerial());
        drawText(320,  90, 0, 0.4, ctx.clrWhite, 0, "%s", getNANDEssentialSerial());
        drawText(320, 100, 0, 0.4, ctx.clrWhite, 0, "%s", getSecinfoSerial());
        drawText(320, 110, 0, 0.4, ctx.clrWhite, 0, "%s", getTWLNSerial());
        
        switch(menustate) {
            case 0:
                drawText(20, 180, 0, 0.5, ctx.clrWhite, 0, "Your Discord name: %s", discordtag);
                drawText(20, 210, 0, 0.5, ctx.clrWhite, 0, "Server address: %s", address);
                if (strlen(address) > 0 && strlen(discordtag) > 0) {
                    drawText(SCREEN_WIDTH_TOP/2, SCREEN_HEIGHT/2, 0, 0.7, ctx.clrWhite, C2D_AlignCenter, "Press on the soap cat to submit");
                } else {
                    drawText(SCREEN_WIDTH_TOP/2, SCREEN_HEIGHT/2+20, 0, 0.7, ctx.clrWhite, C2D_AlignCenter, "Press Y to enter your Discord tag");
                }
                break;
            case 1:
                drawText(SCREEN_WIDTH_TOP/2, SCREEN_HEIGHT*3/4, 0, 0.7, ctx.clrWhite, C2D_AlignCenter, "Submitting...");
                break;
            case 2: 
                drawText(SCREEN_WIDTH_TOP/2, SCREEN_HEIGHT*3/4, 0, 0.5, ctx.clrWhite, C2D_AlignCenter, finaltext);
                break;

        }
        C3D_FrameEnd(0);
        switch(menustate) {
            case 0:
                if ((kDown & KEY_X) && (kDown & KEY_DDOWN)) {
                    enter(address);
                }
                if (kDown & KEY_Y) {
                    enter(discordtag);
                }
                if (strlen(address) > 0 && strlen(discordtag) > 0 && kDown & KEY_TOUCH) {
                    menustate++;
                } 
                break;
            case 2:
                if (kDown & KEY_START)
                    goto deinit;
                break;
            case 1:
                if (getSDEssentialSerial()[0] == '\0') {
                    if (getNANDEssentialSerial()[0] == '\0') {
                        sprintf(finaltext, "essential.exefs not found");
                        menustate++;
                        break;
                    }
                    usenandessential = true;
                }
                initcurl();
                initform(); 
                discordhandleentry(discordtag);
                if (usenandessential) {
                    essentialdataentry();
                } else {
                    fileentry("sdmc:/gm9/out/essential.exefs");
                }
                serialentry("sd", getSDEssentialSerial());
                serialentry("nand", getNANDEssentialSerial());
                serialentry("twln", getTWLNSerial());
                serialentry("secinfo", getSecinfoSerial());
                CURLcode res = submittourl(address);
                if(res != CURLE_OK) {
                    sprintf(finaltext, "curl_easy_perform() failed: %s\n",
                        curl_easy_strerror(res));
                } else {
                    sprintf(finaltext, "Information submitted.");
                }
                exiteverything();
                menustate++;
                break;


        }
        
        
       
    }
fail:
    consoleInit(GFX_TOP, GFX_LEFT);
    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_START)
            goto deinit;
    }
    
deinit:
    if (sheet)
        C2D_SpriteSheetFree(sheet); 
    exitUI();
    socExit();
    romfsExit();
    gfxExit();
}