/**
 * Copyright (c) 2015 Runtime Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <os/os.h>
#include <os/endian.h>

#include <assert.h>
#include <string.h>
#include <stdio.h>

#include <newtmgr/newtmgr.h>
#include <bootutil/image.h>
#include <fs/fs.h>
#include <fs/fsutil.h>
#include <json/json.h>
#include <bsp/bsp.h>

#include "imgmgr/imgmgr.h"
#include "imgmgr_priv.h"

/* XXX share with bootutil */
#define BOOT_PATH		"/boot"
#define BOOT_PATH_MAIN          "/boot/main"
#define BOOT_PATH_TEST          "/boot/test"

#ifdef FS_PRESENT
static int
imgr_read_file(const char *path, struct image_version *ver)
{
    uint32_t bytes_read;
    int rc;

    rc = fsutil_read_file(path, 0, sizeof(*ver), ver, &bytes_read);
    if (rc != 0 || bytes_read != sizeof(*ver)) {
        return -1;
    }
    return 0;
}

static int
imgr_read_test(struct image_version *ver)
{
    return (imgr_read_file(BOOT_PATH_TEST, ver));
}

static int
imgr_read_main(struct image_version *ver)
{
    return (imgr_read_file(BOOT_PATH_MAIN, ver));
}

static int
imgr_write_file(const char *path, struct image_version *ver)
{
    return fsutil_write_file(path, ver, sizeof(*ver));
}

int
imgr_boot_read(struct nmgr_hdr *nmr, struct os_mbuf *req,
  uint16_t srcoff, struct nmgr_hdr *rsp_hdr, struct os_mbuf *rsp)
{
    int rc;
    int prev_set = 0;
    struct image_version ver;
    char str[128];
    int off;

    off = snprintf(str, sizeof(str), "{");
    rc = imgr_read_test(&ver);
    if (!rc) {
        off += imgr_ver_jsonstr(str + off, sizeof(str) - off, "test", &ver);
        prev_set = 1;
    }

    rc = imgr_read_main(&ver);
    if (!rc) {
        if (prev_set) {
            off += snprintf(str + off, sizeof(str) - off, ",");
        }
        off += imgr_ver_jsonstr(str + off, sizeof(str) - off, "main", &ver);
        prev_set = 1;
    }

    rc = imgr_read_ver(bsp_imgr_current_slot(), &ver);
    if (!rc) {
        if (prev_set) {
            off += snprintf(str + off, sizeof(str) - off, ",");
        }
        off += imgr_ver_jsonstr(str + off, sizeof(str) - off, "active", &ver);
    }

    off += snprintf(str + off, sizeof(str) - off, "}");

    rc = nmgr_rsp_extend(rsp_hdr, rsp, str, off);
    if (rc) {
        goto err;
    }

    return 0;
err:
    return rc;
}

int
imgr_boot_write(struct nmgr_hdr *nmr, struct os_mbuf *req,
  uint16_t srcoff, struct nmgr_hdr *rsp_hdr, struct os_mbuf *rsp)
{
    char incoming[64];
    char test_ver_str[28];
    const struct json_attr_t boot_write_attr[2] = {
        [0] = {
            .attribute = "test",
            .type = t_string,
            .addr.string = test_ver_str,
            .len = sizeof(test_ver_str),
        },
        [1] = {
            .attribute = NULL
        }
    };
    int rc;
    const char *end;
    struct image_version ver;

    if (nmr->nh_len > sizeof(incoming)) {
        return OS_EINVAL;
    }
    rc = os_mbuf_copydata(req, srcoff + sizeof(*nmr), nmr->nh_len, incoming);
    if (rc) {
        return OS_EINVAL;
    }

    rc = json_read_object(incoming, boot_write_attr, &end);
    if (rc) {
        return OS_EINVAL;
    }

    rc = imgr_ver_parse(boot_write_attr[0].addr.string, &ver);
    if (rc) {
        return OS_EINVAL;
    }

    fs_mkdir(BOOT_PATH);
    rc = imgr_write_file(BOOT_PATH_TEST, &ver);
    return rc;
}
#endif