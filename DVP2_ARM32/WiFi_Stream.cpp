﻿#include <stdio.h>
#include <stdint.h>
#include "DVPCamera.h"
//#include <pty.h>


int main(int argc, char *argv[])
{
    int ret = 0;

    //forkpty(NULL, NULL, NULL, NULL);

    printf("start...\r\n");

    dvpStatus status;
    dvpUint32 count;
    dvpCameraInfo info[8];   
    dvpHandle h;
    dvpUint32 i,j;
    
    /* Update device list */
    dvpRefresh(&count);
    if (count > 8)
        count = 8;
     
    for (i = 0; i < count; i++)
    {    
        if(dvpEnum(i, &info[i]) == DVP_STATUS_OK)    
        {        
            printf("Camera FriendlyName : %s\r\n", info[i].FriendlyName);    
        }
    }

    /* No device found */
    if (count == 0)
        return 0;

    /* Open the first device */
    status = dvpOpenByName(info[0].FriendlyName, OPEN_NORMAL, &h);
    if (status != DVP_STATUS_OK)
    {
        return 1;
    }

    dvpRegion region;
    double exp;
    float gain;

    /* print ROI information */
    status = dvpGetRoi(h, &region);
    if (status != DVP_STATUS_OK)
    {
        printf("dvpGetRoi failed with err:%d\r\n", status);
        return 1;
    }
    printf("region: x:%d, y:%d, w:%d, h:%d\r\n", region.X, region.Y, region.W, region.H);

    /* print exposure and gain information */
    status = dvpGetExposure(h, &exp);
    if (status != DVP_STATUS_OK)
    {
        printf("dvpGetExposure failed with err:%d\r\n", status);
        return 1;
    }

    status = dvpGetAnalogGain(h, &gain);
    if (status != DVP_STATUS_OK)
    {
        printf("dvpGetAnalogGain failed with err:%d\r\n", status);
        return 1;
    }

    printf("exposure: %lf, gain: %f\r\n", exp, gain);


    uint32_t v;
    dvpSelectionDescr descr;
    dvpFrame frame;
    void *p;

    status = dvpGetQuickRoiSelDescr(h, &descr);
    if (status != DVP_STATUS_OK)
    {
        return 1;
    }

    /* Grab frames for every preset ROI */ 
    for (i=0; i<descr.uCount; i++)
    {
        /* Change image size */
        status = dvpSetQuickRoiSel(h, i);
        if (status != DVP_STATUS_OK)
        {
            ret = 1;
            break;
        }

        /* Start stream */
        status = dvpStart(h);
        if (status != DVP_STATUS_OK)
        {
            ret = 1;
            break;
        }

        /* Grab frames */
        for (j=0; j<100; j++)
        {
            status = dvpGetFrame(h, &frame, &p, 1000);
            if (status != DVP_STATUS_OK)
            {
                ret = 1;
                break;
            }

            /* Show frame information */
            printf("frame:%llu, timestamp:%llu, %d*%d, %dbytes, format:%d\r\n", 
                frame.uFrameID,
                frame.uTimestamp,
                frame.iWidth,
                frame.iHeight,
                frame.uBytes,
                frame.format);

            dvpSavePicture(&frame, p, "WiFi_Stream.jpeg", 100);
   
        }

        /* Stop stream */
        status = dvpStop(h);
        if (status != DVP_STATUS_OK)
        {
            ret = 1;
            break;
        }
    }

    dvpClose(h);
    return ret;
}
