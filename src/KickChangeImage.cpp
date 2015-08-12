#include "KickChangeImage.h"

#include "cinder\app\App.h"

void KickChangeImage::setup(AudioSource* audioSource) 
{
	mAudioSource = audioSource;

	mCurrentImage = gl::Texture::create(loadImage(app::loadAsset("N4GII.jpg")));
}

void KickChangeImage::switchCamera(CameraPersp cam) 
{

}

void KickChangeImage::switchParams(params::InterfaceGlRef params)
{

}

bool KickChangeImage::perspective()
{
	return false;
}

void KickChangeImage::update()
{

}

void KickChangeImage::draw() 
{
	gl::draw(mCurrentImage, app::getWindowBounds());
}

