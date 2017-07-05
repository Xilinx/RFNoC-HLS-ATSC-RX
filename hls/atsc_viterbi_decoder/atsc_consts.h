#ifndef INCLUDED_DTV_ATSC_CONSTS_H
#define INCLUDED_DTV_ATSC_CONSTS_H



// These will go into an mpeg_consts.h once other mod/demods are done
static const int ATSC_MPEG_DATA_LENGTH 	 = 187;
static const int ATSC_MPEG_PKT_LENGTH	 = 188;  // sync + data
static const int ATSC_MPEG_RS_ENCODED_LENGTH = 207;
static const int MPEG_SYNC_BYTE              = 0x47;
static const int MPEG_TRANSPORT_ERROR_BIT    = 0x80; // top bit of byte after SYNC

// ATSC specific constants
static const double ATSC_SYMBOL_RATE         = 4.5e6/286*684;        // ~10.76 MHz
static const double ATSC_DATA_SEGMENT_RATE   = ATSC_SYMBOL_RATE/832; // ~12.935 kHz
static const int ATSC_DATA_SEGMENT_LENGTH    = 832;	                 // includes 4 sync symbols at beginning
static const int ATSC_DSEGS_PER_FIELD	 = 312;	                 // regular data segs / field

#endif /* INCLUDED_DTV_ATSC_CONSTS_H */