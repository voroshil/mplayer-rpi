/*
 * OpenMAX video output driver
 *
 * Copyright (c) 2013 Vladimir Voroshilov <voroshil@gmail.com>
 *
 * This file is part of MPlayer.
 *
 * MPlayer is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * MPlayer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with MPlayer; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "config.h"
#include "fastmemcpy.h"
#include "video_out.h"
#include "video_out_internal.h"
#include "aspect.h"
#include "mp_msg.h"
#include "m_option.h"

static const vo_info_t info =
{
	"OpenMAX video output",
	"null",
	"Vladimir Voroshilov <voroshil@gmail.com>",
	""
};

const LIBVO_EXTERN(openmax)

static int draw_slice(uint8_t *image[], int stride[], int w,int h,int x,int y)
{
  return 0;
}
static void draw_osd(void)
{
}
static void flip_page(void)
{
}
static int draw_frame(uint8_t *src[])
{
  return 0;
}
static void uninit(void)
{
}
static void check_events(void)
{
}
static int preinit(const char *)
{
    if(arg)
    {
	mp_msg(MSGT_VO,MSGL_WARN, MSGTR_LIBVO_NULL_UnknownSubdevice,arg);
	return ENOSYS;
    }
    return 0;
}
static int
query_format(uint32_t format)
{
    if (IMGFMT_IS_HWACCEL(format))
        return 0;
    return VFCAP_CSP_SUPPORTED;
}
static int config(uint32_t width, uint32_t height, uint32_t d_width,
		     uint32_t d_height, uint32_t fullscreen, char *title,
		     uint32_t format)
{
	return 0;
}
static int control(uint32_t request, void *data)
{
  switch (request) {
  case VOCTRL_QUERY_FORMAT:
    return query_format(*((uint32_t*)data));
  }
  return VO_NOTIMPL;
}
