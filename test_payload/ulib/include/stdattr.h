#ifndef _STDATTR_H_
#define _STDATTR_H_

#define ATTR_UMAIN_TEXT __attribute__((section(".umaintext")))

#define ATTR_ULIB1_TEXT __attribute__((section(".ulib1text")))
#define ATTR_ULIB1_DATA __attribute__((section(".ulib1data")))

#define ATTR_ULIB2_TEXT __attribute__((section(".ulib2text")))
#define ATTR_ULIB2_DATA __attribute__((section(".ulib2data")))

#define ATTR_UFREEZONE_TEXT __attribute__((section(".ufreezonetext")))

#endif