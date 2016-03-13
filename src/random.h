#pragma once

#define IA 16807
#define IM 2147483647
#define IQ 127773
#define IR 2836
#define NTAB 32
#define NDIV (1+(IM-1)/NTAB)
#define MAX_RANDOM_RANGE 0x7FFFFFFFUL

#define AM (float)(1.0/IM)
#define EPS (float)1.2e-7
#define RNMX (float)(1.0-EPS)

class C_Random
{
private:
	int        m_idum;
	int        m_iy;
	int        m_iv[NTAB];

	int        GenerateRandomNumber(void)
	{
		int j;
		int k;

		if(m_idum <= 0 || !m_iy)
		{
			if(-(m_idum) < 1) m_idum = 1;
			else m_idum = -(m_idum);

			for(j = NTAB + 7; j >= 0; j--)
			{
				k = (m_idum) / IQ;
				m_idum = IA * (m_idum - k * IQ) - IR * k;
				if(m_idum < 0) m_idum += IM;
				if(j < NTAB) m_iv[j] = m_idum;
			}
			m_iy = m_iv[0];
		}
		k = (m_idum) / IQ;
		m_idum = IA * (m_idum - k * IQ) - IR * k;
		if(m_idum < 0) m_idum += IM;
		j = m_iy / NDIV;
		m_iy = m_iv[j];
		m_iv[j] = m_idum;

		return m_iy;
	}

public:
	void SetSeed(int iSeed)
	{
		m_idum = ((iSeed < 0) ? iSeed : -iSeed);
		m_iy = 0;
	}

	float RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f)
	{
		float fl = AM * (float)this->GenerateRandomNumber();
		if(fl > RNMX) fl = RNMX;
		return (fl * (flMaxVal - flMinVal)) + flMinVal;
	}
};
//===================================================================================================
class CRandom
{
public:
	// set the random seed
	static void RandomSeed(int iSeed)
	{
		typedef void(__cdecl *RandomSeedFn)(int);
		static RandomSeedFn randomSeed = (RandomSeedFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "RandomSeed");
		return randomSeed(iSeed);
	}

	// get a random float
	static float RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f)
	{
		typedef float(__cdecl *RandomFloatFn)(float, float);
		static RandomFloatFn randomFloat = (RandomFloatFn)GetProcAddress(gSignatures.GetModuleHandleSafe("vstdlib.dll"), "RandomFloat");
		return randomFloat(flMinVal, flMaxVal);
	}

	// tier1 md5 random function
	static unsigned int MD5_PseudoRandom(unsigned int seed)
	{
		typedef unsigned int(*MD5_PseudoRandomFn)(unsigned int);
		static DWORD dwLoc = gSignatures.GetClientSignature("E8 ? ? ? ? 83 C4 04 25") + 0x1;
		static MD5_PseudoRandomFn pseudoRandom = (MD5_PseudoRandomFn)((*(PDWORD)(dwLoc)) + dwLoc + 4);
		return pseudoRandom(seed);
	}
};
//===================================================================================================
class IUniformRandomStream
{
public:
	// Sets the seed of the random number generator
	virtual void	SetSeed(int iSeed) = 0;

	// Generates random numbers
	virtual float	RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f) = 0;
	virtual int		RandomInt(int iMinVal, int iMaxVal) = 0;
};
//===================================================================================================
class CUniformRandomStream : public IUniformRandomStream
{
public:
	CUniformRandomStream();

	// Sets the seed of the random number generator
	virtual void	SetSeed(int iSeed);

	// Generates random numbers
	virtual float	RandomFloat(float flMinVal = 0.0f, float flMaxVal = 1.0f);
	virtual int		RandomInt(int iMinVal, int iMaxVal);

private:
	int		GenerateRandomNumber();

	int m_idum;
	int m_iy;
	int m_iv[32];
};
//===================================================================================================