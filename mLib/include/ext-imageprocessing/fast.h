#ifndef FAST_H
#define FAST_H

namespace ml {

    struct FastStorage
    {
        int *scores;
        int scoresLength;

        vec2i *cornersDetect;
        int cornersDetectLength;

        vec2i *cornersFinal;
        int cornersFinalLength;

        int *rowStorage;
        int rowStorageLength;
    };

    namespace fast {
        vec2i* fast9_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        vec2i* fast10_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        vec2i* fast11_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        vec2i* fast12_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        vec2i* fast12_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners, FastStorage &storage);

        vec2i* nonmax_suppression(const vec2i* corners, const int* scores, int num_corners, int* ret_num_nonmax);
        vec2i* nonmax_suppression(const vec2i* corners, const int* scores, int num_corners, int* ret_num_nonmax, FastStorage &storage);
    }

    namespace fast9 {

        int fast9_corner_score(const byte* p, const int pixel[], int bstart);
        vec2i* fast9_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        int* fast9_score(const byte* i, int stride, vec2i* corners, int num_corners, int b);
    }

    namespace fast10 {
        int fast10_corner_score(const byte* p, const int pixel[], int bstart);
        int* fast10_score(const byte* i, int stride, vec2i* corners, int num_corners, int b);
        vec2i* fast10_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
    }

    namespace fast11 {
        int fast11_corner_score(const byte* p, const int pixel[], int bstart);
        vec2i* fast11_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        int* fast11_score(const byte* i, int stride, vec2i* corners, int num_corners, int b);
    }

    namespace fast12 {
        int fast12_corner_score(const byte* p, const int pixel[], int bstart);
        vec2i* fast12_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners);
        vec2i* fast12_detect(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners, FastStorage &storage);
        int* fast12_score(const byte* i, int stride, vec2i* corners, int num_corners, int b);
        int* fast12_score(const byte* i, int stride, vec2i* corners, int num_corners, int b, FastStorage &storage);
    }
}

#endif
