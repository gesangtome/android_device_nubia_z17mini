/*
   Copyright (C) 2017 The LineageOS Project.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are
   met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.
    * Neither the name of The Linux Foundation nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.
   THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
   ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
   BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <fcntl.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <unistd.h>
#include "vendor_init.h"
#include "property_service.h"
#include "log.h"
//#include "util.h"

using android::init::property_set;

char const *description;
char const *fingerprint;

static void init_alarm_boot()
{
    int boot_reason;
    FILE *fp;

    fp = fopen("/proc/sys/kernel/boot_reason", "r");

     if (fp == NULL){
       fclose(fp);
     return;
 }
    fscanf(fp, "%d", &boot_reason);
    fclose(fp);

 /*
     * Setup ro.alarm_boot value to true when it is RTC triggered boot up
     * For existing PMIC chips, the following mapping applies
     * for the value of boot_reason:
     *
     * 0 -> unknown
     * 1 -> hard reset
     * 2 -> sudden momentary power loss (SMPL)
     * 3 -> real time clock (RTC)
     * 4 -> DC charger inserted
     * 5 -> USB charger inserted
     * 6 -> PON1 pin toggled (for secondary PMICs)
     * 7 -> CBLPWR_N pin toggled (for external power supply)
     * 8 -> KPDPWR_N pin toggled (power key pressed)
     */

     if (boot_reason == 3) {
         property_set("ro.alarm_boot", "true");
         property_set("debug.sf.nobootanimation", "1");
 } else {
        property_set("ro.alarm_boot", "false");
     }
}

static int read_file2(const char *fname, char *data, int max_size)
{
    int fd, rc;

    if (max_size < 1)
        return 0;

    fd = open(fname, O_RDONLY);
    if (fd < 0) {
        printf("failed to open '%s'\n", fname);
        return 0;
    }

    rc = read(fd, data, max_size - 1);
    if ((rc > 0) && (rc < max_size))
        data[rc] = '\0';
    else
        data[0] = '\0';
    close(fd);

    return 1;
}

void set_model_name()
{

    char tmp[64];

    /* get soc ID */
    if (read_file2("/sys/devices/soc0/raw_version", tmp, sizeof(tmp))) {

    /* model name setting */
        if (!strncmp(tmp, "1", 1)) {
            property_set("ro.product.model", "NX569J");
        }else{
            property_set("ro.product.model", "NX569H");
        }
    }
}

void init_alarm_boot_properties()
{
            description = "NX569J-user 6.0.1 MMB29M eng.nubia.20171227.001510 release-keys";
            fingerprint = "nubia/NX569J/NX569J:6.0.1/MMB29M/eng.nubia.20170926.235033:user/release-keys";

            property_set("ro.build.description", description);
            property_set("ro.build.fingerprint", fingerprint);
}

void vendor_load_properties()
{

    /* Call method: set_model_name() */
    set_model_name();

    init_alarm_boot();
    init_alarm_boot_properties();
}
