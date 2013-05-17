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

// for sbrk routine
#define _BSD_SOURCE 1
#include <unistd.h>


#include <errno.h>
#include "fastmemcpy.h"
#include "video_out.h"
#include "video_out_internal.h"
#include "aspect.h"
#include "mp_msg.h"
#include "help_mp.h"
#include "m_option.h"



#include "bcm_host.h"

#define USE_VCHIQ_ARM
#include "interface/vmcs_host/vcilcs.h"
#include "interface/vmcs_host/vchost.h"
#include "interface/vmcs_host/vcilcs_common.h"
#include "IL/OMX_Component.h"

static const vo_info_t info =
{
	"OpenMAX video output",
	"openmax",
	"Vladimir Voroshilov <voroshil@gmail.com>",
	""
};

const LIBVO_EXTERN(openmax)
typedef struct {
  int vcos_initialized;
  VCOS_ONCE_T once;
  ILCS_SERVICE_T *ilcs_service;
  int active_handle_count;
} vo_openmax_priv_t;

static vo_openmax_priv_t priv;

static VCOS_MUTEX_T lock;
static void initOnce(void)
{
    VCOS_STATUS_T status;
    status = vcos_mutex_create(&lock, VCOS_FUNCTION);
    vcos_demand(status == VCOS_SUCCESS);
}
static int omx_init(vo_openmax_priv_t* priv)
{
  VCOS_STATUS_T status;
  VCHI_INSTANCE_T initialize_instance;
  VCHI_CONNECTION_T *connection;
  ILCS_CONFIG_T config;

  status = vcos_once(&priv->once, initOnce);
  vcos_demand(status == VCOS_SUCCESS);

  vcos_mutex_lock(&lock);  
  if (priv->vcos_initialized > 0)
  {
    priv->vcos_initialized++;
    vcos_mutex_unlock(&lock);
    return 0;
  }
  
  vc_host_get_vchi_state(&initialize_instance, &connection);
  vcilcs_config(&config);
  priv->ilcs_service = ilcs_init((VCHIQ_INSTANCE_T)initialize_instance, (void**)&connection, &config, 0);
  if (priv->ilcs_service == NULL)
  {
    mp_msg(MSGT_VO, MSGL_ERR, "ilcs init error: %x\n", status);
    vcos_mutex_unlock(&lock);
    return -1;
  }
  priv->vcos_initialized = 1;
  vcos_mutex_unlock(&lock);
  return 0;
}
static void omx_uninit(vo_openmax_priv_t* priv)
{
  if (priv->vcos_initialized ==0)
    return;

  if (priv->vcos_initialized==1 && priv->active_handle_count >0)
    return;

  vcos_mutex_lock(&lock);
  priv->vcos_initialized--;
  if (priv->vcos_initialized == 0)
  {
    ilcs_deinit(priv->ilcs_service);
    priv->ilcs_service = NULL;
  }
  vcos_mutex_unlock(&lock);
}


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
  omx_uninit(&priv);
}
static void check_events(void)
{
}
static int preinit(const char * arg)
{
    if(arg)
    {
	mp_msg(MSGT_VO,MSGL_WARN, MSGTR_LIBVO_NULL_UnknownSubdevice,arg);
	return ENOSYS;
    }

    memset(&priv, 0, sizeof(priv));

   priv.once = VCOS_ONCE_INIT;

    // BCM chip initialization. Must be called before any operation
    bcm_host_init();

    if (omx_init(&priv) != 0)
        return ENOSYS;
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
