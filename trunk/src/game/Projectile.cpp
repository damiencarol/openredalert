// Projectile.cpp
// 1.0

//    This file is part of OpenRedAlert.
//
//    OpenRedAlert is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, version 2 of the License.
//
//    OpenRedAlert is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with OpenRedAlert.  If not, see <http://www.gnu.org/licenses/>.

#include "Projectile.h"

#include <string>
#include <vector>

#include "ProjectileData.h"
#include "ProjectileDataList.h"
#include "include/Logger.h"
#include "video/ImageNotFound.h"
#include "video/SHPImage.h"

using std::string;
using std::vector;

extern Logger * logger;

Projectile::Projectile(string pname, ProjectileDataList* data, vector<SHPImage*>* imagePool)
{
	// Get the Data of this projectile
	ProjectileData* lProjectileData = data->getData(pname);
		
	// Assign the Data
	this->lnkProjectileData = lProjectileData;
	
	
	// Get image name
	string lImage = this->lnkProjectileData->getImage();
	
	// Load the image in the pool 
	if (lImage!= "none"){
		
		SHPImage* temp = 0;
		imagenum = 0;
		
		imagenum = imagePool->size()<<16;
		try
		{
			lImage += ".shp";
			temp = new SHPImage(lImage.c_str(), -1);
		}
		catch (ImageNotFound&)
		{
			logger->error("Image %s not found during loading of %s projectile\n", lImage.c_str(), pname.c_str());
		}
		// stack the image
		imagePool->push_back(temp);
		
		
		
		if (this->lnkProjectileData->getRotates()!= 0)
		{
			if (temp != 0)
			{
				rotationimgs = temp->getNumImg();
			}
			else
			{
				rotationimgs = 0;
			}
		}

	}
}

Projectile::~Projectile()
{
}

Uint32 Projectile::getImageNum()
{
	return imagenum;
}

bool Projectile::AntiSubmarine()
{
	return this->lnkProjectileData->getASW();
}

bool Projectile::AntiGround()
{
	return this->lnkProjectileData->getAG();
}

bool Projectile::AntiAir()
{
	if (this->lnkProjectileData->getAA()==1)
	{
		return true;
	}
	return false;
}

bool Projectile::getInaccurate()
{
	if (this->lnkProjectileData->getInaccurate()==1)
	{
		return true;
	}
	return false;
}

bool Projectile::doesRotate()
{
	if (this->lnkProjectileData->getRotates()==1)
	{
		return true;
	}
	return false;
}
