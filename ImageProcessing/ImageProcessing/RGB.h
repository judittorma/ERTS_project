#pragma once

struct RGB
{
	// members
	float R; // red
	float G; // green
	float B; // blue

	// constructors
	RGB() : R(0), G(0), B(0) {}
	RGB(float r, float g, float b) : R(r), G(g), B(b) {}

	// operations

	bool operator ==(const RGB &p) const
	{
		return p.R == R && p.G == G && p.B == B;
	}

	bool operator !=(const RGB &p) const
	{
		return p.R != R || p.G != G || p.B != B;
	}

	RGB& operator *=(const RGB &p)
	{
		R *= p.R, G *= p.G, B *= p.B;
		return *this;
	}

	RGB& operator +=(const RGB &p)
	{
		R += p.R, G += p.G, B += p.B;
		return *this;
	}

	friend float& operator +=(float &f, const RGB p)
	{
		f += (p.R + p.G + p.B) / 3.f;
		return f;
	}
};
