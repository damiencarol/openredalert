// Path.cpp
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

#include "Path.h"

#include <cstdlib>
#include <queue>
#include <vector>

#include "CnCMap.h"
#include "KeyComp.h"
#include "FibHeapEntry.h"
#include "TileRef.h"
#include "Unit.h"
using std::min;

namespace p 
{
	extern CnCMap * ccmap;
}

/** 
 * Path constructor
 * 
 * @param Unit		The unit for witch a path must be found
 * @param crBeg		The begin position for the unit
 * @param crEnd		The end or target position for this unit
 * @param max_dist	The maximum distance to the target where we can stop moving (i think??)
**/
Path::Path(Unit *Unit, Uint32 crBeg, Uint32 crEnd, Uint8 max_dist) : std::stack<Uint8>()
{
    Uint16 startposx;
    Uint16 startposy;
    Uint16 stopposx;
    Uint16 stopposy;
    FibHeapEntry** Nodes;
    Uint8 *ed;
    FibHeapEntry *pU, *pV;
    FibHeapEntry *pReuse;
    FibHeapEntry *retired;
    std::priority_queue<FibHeapEntry*, std::vector<FibHeapEntry *>, KeyComp> PQ;
    Uint32 crU, crV;
    Uint16	crX, crY,	// Current x and y pos
			ncrX, ncrY;	// New x and y pos

    Uint32 dwCost;
    int i;
    TileRef *cellTempU, *cellTempV;
    Uint16 diffx, diffy;
    Uint8 edp;
    Uint32 cmincr;
    Uint32 cminh;

    Uint16 mapsize = p::ccmap->getWidth()*p::ccmap->getHeight();

	// Get the starting and ending x and y positions
    p::ccmap->translateFromPos(crBeg, &startposx, &startposy);
    p::ccmap->translateFromPos(crEnd, &stopposx, &stopposy);

    /* When making optimised builds, the compiler whines about these variables
     * being possibly uninitialised.
     */
    crU = crV = 0;
    ncrX = ncrY = 0;

    Nodes = new FibHeapEntry*[mapsize];

    ed = new Uint8[mapsize];


    //for( i = 0; i < mapsize; i++ )
    //Nodes[i] = NULL;
    memset(Nodes, 0, mapsize*sizeof(FibHeapEntry*));

    cellTempU = new TileRef;
    cellTempU->crx = startposx;
    cellTempU->cry = startposy;
    cellTempU->g = 0;
    diffx = abs( startposx - stopposx );
    diffy = abs( startposy - stopposy );
    cellTempU->h = min(diffx, diffy)*14 + abs( diffx - diffy )*10;

    Nodes[crBeg] = pU = new FibHeapEntry(cellTempU, cellTempU->h);

    cmincr = crBeg;
    cminh = cellTempU->h;

    PQ.push(pU);

    retired = new FibHeapEntry(NULL, 0);
    pReuse = NULL;

    while( !PQ.empty() ){
        pU = PQ.top();
        PQ.pop();
        cellTempU = pU->getValue();
        crX = cellTempU->crx;
        crY = cellTempU->cry;
        crU = p::ccmap->translateToPos(crX, crY);

        if( cellTempU->h < cminh ) {
            cminh = cellTempU->h;
            cmincr = crU;
        }

        if( cellTempU->h <= ((Uint32)max_dist)*10 || cellTempU->g >= 1000)
            /* desired min dist to target, 0 to go all the way. Currently the length of the path is limited to 100 */
            break;

		// Walk in all directions
		for( edp = 0; edp < 8; edp++ ) {

			switch( edp ) {
				case 0:
					if( crY == 0 )
						dwCost = 0xffff;
					else {
						ncrX = crX;
						ncrY = crY-1;
						crV = crU - p::ccmap->getWidth();
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*10;
					}
					break;
				case 1:
					if( crY == 0 || crX == (p::ccmap->getWidth()-1) )
						dwCost = 0xffff;
					else {
						ncrX = crX+1;
						ncrY = crY-1;
						crV = crU - p::ccmap->getWidth() + 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*14;
					}
					break;
				case 2:
					if( crX == (p::ccmap->getWidth()-1) )
						dwCost = 0xffff;
					else {
						ncrY = crY;
						ncrX = crX+1;
						crV = crU + 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*10;
					}
					break;
				case 3:
					if( crY == (p::ccmap->getHeight()-1) || crX == (p::ccmap->getWidth()-1) )
						dwCost = 0xffff;
					else {
						ncrX = crX+1;
						ncrY = crY+1;
						crV = crU + p::ccmap->getWidth() + 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*14;
					}
					break;
				case 4:
					if( crY == (p::ccmap->getHeight()-1) )
						dwCost = 0xffff;
					else {
						ncrX = crX;
						ncrY = crY+1;
						crV = crU + p::ccmap->getWidth();
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*10;
					}
					break;
				case 5:
					if( crY == (p::ccmap->getHeight()-1) || crX == 0 )
						dwCost = 0xffff;
					else {
						ncrX = crX-1;
						ncrY = crY+1;
						crV = crU + p::ccmap->getWidth() - 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*14;
					}
					break;
				case 6:
					if( crX == 0 ){
						dwCost = 0xffff;
                    } else {
						ncrY = crY;
						ncrX = crX-1;
						crV = crU - 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*10;
					}
					break;
				case 7:
					if( crY == 0 || crX == 0 ){
                        dwCost = 0xffff;
                    } else {
						ncrX = crX-1;
						ncrY = crY-1;
						crV = crU - p::ccmap->getWidth() - 1;
						dwCost = cellTempU->g + p::ccmap->getCost(crV, Unit)*14;
					}
					break;
				default:
					dwCost = 0xffff;
					break;
			}

			if( dwCost >= 0xffff ){
				continue;
            }
			pV = Nodes[crV];

			/* Retired */
			if( pV == retired ){
				continue;

			/* Not visited */
			}else if( pV == NULL ) {

				if( pReuse != NULL ) {
					pV = pReuse;
					pReuse = NULL;
					cellTempV = pV->getValue();
				} else {
					cellTempV = new TileRef;
					pV = new FibHeapEntry(cellTempV, 0);
				}

				Nodes[crV] = pV;
				ed[crV] = edp;

				cellTempV->crx = ncrX;
				cellTempV->cry = ncrY;
				cellTempV->g = dwCost;
				diffx = abs( ncrX - stopposx );
				diffy = abs( ncrY - stopposy );
				cellTempV->h = min(diffx, diffy)*14 + abs( diffx - diffy )*10;

				pV->setKey(cellTempV->g + cellTempV->h);

				PQ.push(pV);
			} else if( dwCost <= pV->getValue()->g ) {
				if( dwCost == pV->getValue()->g ) {
			    	/*
                    if( rand() <= RAND_MAX/2 ){
					    ed[crV] = edp;
                    }
                    */
					continue;
				}
				pV->getValue()->g = dwCost;
				ed[crV] = edp;

				/* Decrease the key */
				pV->setKey(dwCost + pV->getValue()->h);
			}
		}

		Nodes[crU] = retired;
		if( pReuse != NULL ) {
			if (cellTempU != NULL){
				delete cellTempU;
            }
			cellTempU = NULL;

			if (pU != NULL){
				delete pU;
            }
			pU = NULL;
		} else
			pReuse = pU;

//		break;

    } /* while done */

    if( crU != crEnd ) {
        crEnd = cmincr;
    }

    if( pReuse != NULL ) {
		if ((TileRef *)pReuse->getValue() != NULL){
			delete (TileRef *)pReuse->getValue();
        }

		if (pReuse != NULL){
			delete pReuse;
        }
		pReuse = NULL;
	}

	for( i = 0; i < mapsize; i++ ){
		if( Nodes[i] != NULL && Nodes[i] != retired ) {
			if ((TileRef *)Nodes[i]->getValue() != NULL){
				delete (TileRef *)Nodes[i]->getValue();
            }
			if (Nodes[i] != NULL){
				delete Nodes[i];
            }
			Nodes[i] = NULL;
		}
    }
    if (Nodes != NULL){
		delete[] Nodes;
    }
	Nodes = NULL;
	if (retired != NULL){
		delete retired;
    }
    retired = NULL;


	if( crEnd != crBeg ) {
		crV = crEnd;

		while( crV != crBeg ) {
            push(ed[crV]);
            switch( ed[crV] ) {
            case 0:
                crV = crV + p::ccmap->getWidth();
                break;
            case 1:
                crV = crV + p::ccmap->getWidth() - 1;
                break;
            case 2:
                crV = crV - 1;
                break;
            case 3:
                crV = crV - p::ccmap->getWidth() - 1;
                break;
            case 4:
                crV = crV - p::ccmap->getWidth();
                break;
            case 5:
                crV = crV - p::ccmap->getWidth() + 1;
                break;
            case 6:
                crV = crV + 1;
                break;
            case 7:
                crV = crV + p::ccmap->getWidth() + 1;
                break;
            default:
				if (ed != NULL){
					delete[] ed;
                }
				ed = NULL;
                return;
            }
        }

		if (ed != NULL){
			delete[] ed;
        }
		ed = NULL;
        return;
    } else {
		if (ed != NULL){
			delete[] ed;
        }
		ed = NULL;
        return;
    }
}

/**
 * Empty the path 
 */
Path::~Path()
{
    while(!empty())
    {
        pop();
    }
}
