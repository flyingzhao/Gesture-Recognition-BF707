/*****************************************************************************
 * imcatchandshow.h
 *****************************************************************************/

#ifndef __HANDRECOG_H__
#define __HANDRECOG_H__

/* Add your custom header content here */
extern void getgrayscale(unsigned char *data);
extern void yuv422torgb565(unsigned char *imAddrstart);
extern void imdilate(unsigned char *imresult,unsigned char *imAddrstart,unsigned char *mask);
extern void imerode(unsigned char *imresult,unsigned char *imAddrstart,unsigned char *mask);


#endif /* __IMCATCHANDSHOW_H__ */
