/*******************************************************************************
*   Copyright 2013 EPFL                                                        *
*                                                                              *
*   This file is part of metroscope.                                           *
*                                                                              *
*   Metroscope is free software: you can redistribute it and/or modify         *
*   it under the terms of the GNU General Public License as                    *
*   published by the Free Software Foundation, either version 3 of the         *
*   License, or (at your option) any later version.                            *
*                                                                              *
*   Metroscope is distributed in the hope that it will be useful,              *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of             *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
*   GNU General Public License for more details.                               *
*                                                                              *
*   You should have received a copy of the GNU General Public License          *
*   along with Metroscope.  If not, see <http://www.gnu.org/licenses/>.        *
*******************************************************************************/

/* 
 * File:   TagTranscoder.h
 * Author: zufferey
 *
 * Created on August 5, 2010, 10:05 PM
 */

#ifndef _TAGTRANSCODER_H
#define	_TAGTRANSCODER_H

struct tag_info_t {
    int id; // id of the tag
    int tracking_id;
    int xor_id; // id after applying XOR mask
    long crc; // id after computing the crc
    unsigned char fec[36]; // id after computing the fec, used to draw the tag
};

class TagTranscoder {
public:
    TagTranscoder(int pBitsId, int pBitsCrc, int pBitsFec, const char *pXorMask, const char *pCrcPoly);
    virtual ~TagTranscoder();

    int getMaxTagsNumber() const { return m_maxTagsNumber; }
    int getNTotalTagsTracked() const { return m_nTotalTagsTracked; }
    int getTrackingId(int pId) const { return m_tagIdToTrackingIdTable[pId]; }
    int getTagId(int pTrackingId) const { return m_trackedTagsTable[pTrackingId].id; }

    int addTagToTrackingList(int id);

    bool decode(const unsigned char *data, tag_info_t **tag);

    int getTagEncodedId(int tagId, unsigned char *data);

private:
    void encode(tag_info_t *tag);
    int computeCRC(tag_info_t *tag);
    int computeFEC(tag_info_t *tag);

    bool viterbi(const unsigned char *encoded_id,
    		const unsigned char *tag_data, tag_info_t **tag);

    static void bin2int(const unsigned char *bin, int *out, int size);
    static unsigned long binstr2int(const char *bin);

private:

    int m_bitsId;
    int m_bitsCrc;
    int m_bitsFec;
    unsigned long m_xorMask;
    unsigned long m_crcPoly;
    int m_maxTagsNumber;

    // table of tracked tags (index is trackingId), unused slots point to NULL
    tag_info_t *m_trackedTagsTable;

    // conversion table from tag id to tracking id (-1 if not tracked)
    int *m_tagIdToTrackingIdTable;

    int m_bitsBeforePuncturing;
    int m_bitsAfterPuncturing;

    // total number of currently tracked tags
    int m_nTotalTagsTracked;

    // puncturing matrix
    unsigned char *m_puncturing;

    // decoding - fec decodes only the 20 first bits: enough to decode potential IDs
    // greatly saves time compared to the full Viterbi algorithm
    unsigned char *m_dec_fec_id;

    // tables used to save state at each level: avoids recursive calls that would be slower
    // there are 10 levels, corresponding to the 10 bits that have to be extracted, 11 here since
    // some values are referred to in previous levels
    int *m_hamming_dist; // hamming distance at level
    int *m_exploration_level; // number of tests done at this level (max 2, then back to previous level)
    int *m_fec_state;
    unsigned char *m_fec_decoded_id;

    struct fec_state {
        int output[2];
        int next_state[2];
    };

    struct fec_state_dec {
        int input[2];
        int output[2];
        int next_state[2];
    };

    fec_state m_fec_fsm[4];
    fec_state_dec m_fec_dec[4];

};

#endif	/* _TAGTRANSCODER_H */

