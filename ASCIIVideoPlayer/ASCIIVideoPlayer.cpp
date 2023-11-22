#include "VideoPlayer.h"



int main()
{
	// setup
	Renderer::setWidth(200);
	Renderer::allocBuffers();

	VideoPlayer::init();

	VideoPlayer::playVideo("./videos/badapple!.mp4");

	Renderer::freeBuffers();
	return 0;
}
