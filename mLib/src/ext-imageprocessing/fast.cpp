namespace ml {

namespace fast {

vec2i* fast9_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners)
{
	vec2i* corners;
	int num_corners;
	int* scores;
	vec2i* nonmax;

	corners = ::ml::fast9::fast9_detect(im, xsize, ysize, stride, b, &num_corners);
    scores = ::ml::fast9::fast9_score(im, stride, corners, num_corners, b);
	nonmax = nonmax_suppression(corners, scores, num_corners, ret_num_corners);

	free(corners);
	free(scores);

	return nonmax;
}

vec2i* fast10_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners)
{
	vec2i* corners;
	int num_corners;
	int* scores;
	vec2i* nonmax;

    corners = ::ml::fast10::fast10_detect(im, xsize, ysize, stride, b, &num_corners);
    scores = ::ml::fast10::fast10_score(im, stride, corners, num_corners, b);
	nonmax = nonmax_suppression(corners, scores, num_corners, ret_num_corners);

	free(corners);
	free(scores);

	return nonmax;
}

vec2i* fast11_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners)
{
	vec2i* corners;
	int num_corners;
	int* scores;
	vec2i* nonmax;

    corners = ::ml::fast11::fast11_detect(im, xsize, ysize, stride, b, &num_corners);
    scores = ::ml::fast11::fast11_score(im, stride, corners, num_corners, b);
	nonmax = nonmax_suppression(corners, scores, num_corners, ret_num_corners);

	free(corners);
	free(scores);

	return nonmax;
}

vec2i* fast12_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners)
{
	vec2i* corners;
	int num_corners;
	int* scores;
	vec2i* nonmax;

    corners = ::ml::fast12::fast12_detect(im, xsize, ysize, stride, b, &num_corners);
    scores = ::ml::fast12::fast12_score(im, stride, corners, num_corners, b);
	nonmax = nonmax_suppression(corners, scores, num_corners, ret_num_corners);

	free(corners);
	free(scores);

	return nonmax;
}

vec2i* fast12_detect_nonmax(const byte* im, int xsize, int ysize, int stride, int b, int* ret_num_corners, FastStorage &storage)
{
    vec2i* corners;
    int num_corners;
    int* scores;
    vec2i* nonmax;

    corners = ::ml::fast12::fast12_detect(im, xsize, ysize, stride, b, &num_corners, storage);
    scores = ::ml::fast12::fast12_score(im, stride, corners, num_corners, b, storage);
    nonmax = nonmax_suppression(corners, scores, num_corners, ret_num_corners, storage);

    return nonmax;
}

}

}