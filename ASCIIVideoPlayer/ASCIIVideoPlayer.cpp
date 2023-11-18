#include "VideoPlayer.h"

int main()
{
	// setup
	Renderer::setWidth(200);
	Renderer::allocBuffers();

	VideoPlayer::init();
}
