#ifndef CURLNOISE_H
#define CURLNOISE_H

#include <perlin/perlin.h>
#include "line.h"

#include <vector>

namespace curlnoise
{

typedef perlin::Point2< float > Float2;

class Noise2D
{
public:

	Noise2D( perlin::Noise2D* noise, std::vector< Line* > boundaries )
	 : m_noise( noise ),
	   m_boundaries( boundaries ) {}

	~Noise2D()
	{
		delete m_noise;
	}

	Float2 generate( float posX, float posY ) const
	{
		return generate( Float2( posX, posY ) );
	}

	Float2 generate( const Float2& p ) const
	{
		std::vector< Line* >::const_iterator it = m_boundaries.begin();
		std::vector< Line* >::const_iterator end = m_boundaries.end();

		// TODO: Fix
		float minDistanceSq = 10000.0f;

		for ( ; it != end; ++it )
		{
			float distanceSq = (*it)->distanceSq( p );
			if ( distanceSq < minDistanceSq ) 
				minDistanceSq = distanceSq;
		}

		float scale = 1.0f;
		float r = minDistanceSq;
		r /= 4.0;
		if ( r < 1.0f )
		{
			scale = (15.0f/8.0f) * r - ( 10.0f/8.0f ) * r * r * r + ( 3.0f/8.0f ) * r * r * r * r * r;
		}

		float eps = 0.01;

		float dy = scale * ( m_noise->generate( p.x, p.y + eps ) - m_noise->generate( p.x, p.y - eps ) );
		float dx = scale * ( m_noise->generate( p.x + eps, p.y ) - m_noise->generate( p.x - eps, p.y ) );

		return Float2( dy, -dx );
	}

private:

	perlin::Noise2D* m_noise;
	std::vector< Line* > m_boundaries;
};


class CurlFactory
{
public:

	CurlFactory( perlin::NoiseFactory factory )
	 : m_noiseFactory( factory ) {}

	Noise2D* create2D( std::vector< Line* > boundaries )
	{
		perlin::Noise2D* perlinNoise = m_noiseFactory.create2D();

		return new Noise2D( perlinNoise, boundaries );
	}

private:

	perlin::NoiseFactory m_noiseFactory;

};

}; // namespace curlnoise

#endif // CURLNOISE_H

