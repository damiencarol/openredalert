#include "include/Input.h"

#include <string>

#include "SDL/SDL_events.h"
#include "SDL/SDL_timer.h"

#include "Cursor.h"
#include "Selection.h"
#include "Sidebar.h"

#include "include/ccmap.h"
#include "include/dispatcher.h"
#include "include/Logger.h"
#include "include/PlayerPool.h"
#include "audio/SoundEngine.h"
#include "game/Unit.h"
#include "game/StructureType.h"
#include "game/ExplosionAnim.h"
#include "game/InfantryGroup.h"
#include "video/ImageCache.h"
#include "video/MessagePool.h"
#include "include/config.h"
#include "game/Player.h"
#include "game/ConStatus.h"
#include "game/BQueue.h"
#include "game/UnitAndStructurePool.h"

/* The defines were introduced in SDL 1.2.5, but should work with all
 * versions since mousewheel support was added.
 */
#ifndef SDL_BUTTON_WHEELUP
#define SDL_BUTTON_WHEELUP 4
#endif
#ifndef SDL_BUTTON_WHEELDOWN
#define SDL_BUTTON_WHEELDOWN 5
#endif

bool Input::drawing = false;
bool Input::minimapEnabled = false;
SDL_Rect Input::markrect;


namespace pc {
    extern ConfigType Config;
}
extern Logger * logger;


/** Constructor, sets up the input handeler.
 * @param the sidebar.
 * @param the map.
 * @param the width of the screen.
 * @param the height of the screen.
 */
Input::Input(Uint16 screenwidth, Uint16 screenheight, SDL_Rect *maparea) :
    width(screenwidth), height(screenheight), done(0), donecount(0),
    finaldelay(getConfig().finaldelay), gamemode(p::ccmap->getGameMode()),
    maparea(maparea), tabwidth(pc::sidebar->getTabLocation()->w),
    tilewidth(p::ccmap->getMapTile(0)->w), lplayer(p::ppool->getLPlayer()),
    kbdmod(k_none), lmousedown(m_none), rmousedown(m_none),
    currentaction(a_none), rcd_scrolling(false), sx(0), sy(0)
{
    if (lplayer->isDefeated()) {
        logger->gameMsg("%s line %i: TEMPORARY FEATURE: Free MCV because of no initial", __FILE__, __LINE__);
        logger->gameMsg("units or structures.  Fixing involves adding triggers.");
        logger->gameMsg("Don't right click :-)");
        currentaction = a_place;
        placeposvalid = true;
        temporary_place_unit = true;
        strcpy(placename,"MCV");
        lplayer->setVisBuild(Player::SOB_BUILD,true);
        lplayer->setVisBuild(Player::SOB_SIGHT, true);
        placetype = p::uspool->getStructureTypeByName("sbag");
    }

    // Reset of special buttons
    for (int i = 0; i < 4; i++) {
    	special_but_was_down[i] = false;
    }    
    
    // create selected object
    this->selected = new Selection(); 
}

Input::~Input() {
	// Free selected object
	delete this->selected;
}

/** 
 * Method to handle the input events. 
 */
void Input::handle()
{
    SDL_Event event;
    int mx, my;
    Uint8 sdir, radarstat;
    Uint8* keystate;
    static ConfigType config = getConfig();

    while ( SDL_PollEvent(&event) ) {
        switch (event.type) {
        case SDL_MOUSEBUTTONDOWN:
            // Mousewheel scroll up
            if (event.button.button == SDL_BUTTON_WHEELUP) {
                pc::sidebar->ScrollSidebar(true);
                // Mousewheel scroll down
            } else if (event.button.button == SDL_BUTTON_WHEELDOWN) {
                pc::sidebar->ScrollSidebar(false);
            } else {
                mx = event.button.x;
                my = event.button.y;
                if( mx >= maparea->x && mx < maparea->x + maparea->w &&
                        my >= maparea->y && my < maparea->y + maparea->h ) {
                    if( !rcd_scrolling && (event.button.button == SDL_BUTTON_LEFT )) {
                        // start drawing the selection square if in map
                        lmousedown = m_map;
                        markrect.x = mx;
                        markrect.y = my;
                    } else if( event.button.button == SDL_BUTTON_RIGHT ) {
                        rmousedown = m_map;
                        sx = mx;
                        sy = my;
                        rcd_scrolling = true;
                    }
                }
                if (mx > width-tabwidth && pc::sidebar->getVisible()) {
                    clickSidebar(mx, my, event.button.button == SDL_BUTTON_RIGHT);
                }
            }
            break;
        case SDL_MOUSEBUTTONUP:
		for (int i =1; i < 4; i++){
			if (pc::sidebar->getSpecialButtonState(i) == 1){ // button down
				if (special_but_was_down[i]){
					pc::sidebar->setSpecialButtonState(i, 0); // Set button up
					special_but_was_down[i] = false;
				}else{
					pc::sidebar->setSpecialButtonState(i, 2); // Set button down
					special_but_was_down[i] = true;
				}
			}
		}
		// If the right mouse is clicked and a special mouse button was down --> set it to up again
		if (event.button.button == SDL_BUTTON_RIGHT) {
			for (int i =1; i < 4; i++){
				if (pc::sidebar->getSpecialButtonState(i) == 2){ // button down
					printf ("%s line %i: Right mouse and special button %i is down --> move up\n", __FILE__, __LINE__, i);
					pc::sidebar->setSpecialButtonState(i, 0); // Set button up
					special_but_was_down[i] = false;
				}
			}
		}

            mx = event.button.x;
            my = event.button.y;
            pc::sidebar->ResetButton();
            if (event.button.button == SDL_BUTTON_LEFT) {
                if( drawing ) {
                    selectRegion();
                } else if( my >= maparea->y ) {
                    if( my < maparea->y + maparea->h ) { /* map */
                        if( mx >= maparea->x && mx < maparea->x+maparea->w ) {
                            if (lmousedown == m_map) {
                                clickMap(mx, my);
                            }
                        }
                    }
                } else if( my < pc::sidebar->getTabLocation()->h ) { /* clicked a tab */
                    if( mx < tabwidth ) {
                        //printf("Options menu\n");
						pc::Config.pause = true;
                    } else if( mx > width-tabwidth ) {
                        pc::sidebar->ToggleVisible();
                    }
                }
                /* should also check if minimap is clicked, */

                /* We can't be drawing now */
                drawing = false;
                lmousedown = m_none;
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                // not moved mouse
                if ((mx < (sx+10)) && ((mx+10) > sx) && (my < (sy+10))  && ((my+10) > sy)) {
                    currentaction = a_none;
                    selected->clearSelection();
                } else {}
                rmousedown = m_none;
                rcd_scrolling = false;
            }
            break;
            // A key has been pressed or released
        case SDL_KEYDOWN:
            /* If it wasn't a press, ignore this event */
            if( event.key.state != SDL_PRESSED )
                break;
            if (event.key.keysym.sym == config.bindablekeys[KEY_SIDEBAR]) {
                pc::sidebar->ToggleVisible();
            } else if (event.key.keysym.sym == config.bindablekeys[KEY_STOP]) {
                selected->stop();
            } else if (event.key.keysym.sym == config.bindablekeys[KEY_ALLY]) {
                if (gamemode == 0) {
                    break;
                }
                Player* tplayer = p::ppool->getPlayer(selected->getOwner());
                if (tplayer == 0) {
                    break;
                }
                if (lplayer->allyWithPlayer(tplayer)) {
                    logger->gameMsg("%s allied with player %s",lplayer->getName(),tplayer->getName());
                } else {
                    if (lplayer->unallyWithPlayer(tplayer)) {
                        logger->gameMsg("%s declared war on player %s",lplayer->getName(),tplayer->getName());
                    } else {
                        // tried to unally with self
                    }
                }
            } else {
                Uint8 k_temp = kbdmod;
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
					pc::Config.pause = true;
                    //done = 1;
                    break;
                case SDLK_RSHIFT:
                case SDLK_LSHIFT:
                    k_temp |= k_shift;
                    kbdmod = (keymod)k_temp;
                    break;
                case SDLK_RCTRL:
                case SDLK_LCTRL:
                    k_temp |= k_ctrl;
                    kbdmod = (keymod)k_temp;
                    break;
                case SDLK_RALT:
                case SDLK_LALT:
                    k_temp |= k_alt;
                    kbdmod = (keymod)k_temp;
                    break;
                case SDLK_1:
                case SDLK_2:
                case SDLK_3:
                case SDLK_4:
                case SDLK_5:
                case SDLK_6:
                case SDLK_7:
                case SDLK_8:
                case SDLK_9:
                case SDLK_0: {
                    Uint8 groupnum = event.key.keysym.sym-48;
                    if (kbdmod == k_ctrl) {
                        if (selected->saveSelection(groupnum)) {
                            logger->gameMsg("Saved group %i",groupnum);
                        } else {
                            logger->gameMsg("Not saved group %i",groupnum);
                        }
                    } else if (kbdmod != k_alt) { // shift or none
                        bool success;
                        if (kbdmod == k_none) {
                            success = selected->loadSelection(groupnum);
                        } else {
                            success = selected->mergeSelection(groupnum);
                        }
                        if (success) {
                            logger->gameMsg("Group %i selected",event.key.keysym.sym-48);
                            Unit* tmpunit = selected->getRandomUnit();
                            if (tmpunit != 0) {
                                UnitType* utype = static_cast<UnitType*>(tmpunit->getType());
                                pc::sfxeng->PlaySound(utype->getRandTalk(TB_report));
                            }
                        }
                    } }
                    break;
                case SDLK_F1:
                case SDLK_F2:
                case SDLK_F3:
                case SDLK_F4:
                case SDLK_F5: {
                    Uint8 locnum = event.key.keysym.sym-282;
                    if (kbdmod == k_ctrl) {
                        p::ccmap->storeLocation(locnum);
                    } else if (kbdmod == k_none) {
                        p::ccmap->restoreLocation(locnum);
                    } }
                    break;
                case SDLK_F7:
                    logger->gameMsg("MARK @ %i",SDL_GetTicks());
                    logger->debug("Mark placed at %i\n",SDL_GetTicks());
                    break;
                case SDLK_F8:
                    p::uspool->showMoves();
                    break;
                case SDLK_v:
                    if (!lplayer->canSeeAll()) {
                        lplayer->setVisBuild(Player::SOB_SIGHT, true);
                        logger->gameMsg("Map revealed");
                    }
                    break;
                case SDLK_c:
                    if (!lplayer->canBuildAny()) {
                        lplayer->setVisBuild(Player::SOB_BUILD, true);
                        logger->gameMsg("Build (nearly) anywhere");
                    }
                    break;
                case SDLK_b:
                    if (!lplayer->canBuildAll()) {
                        p::uspool->preloadUnitAndStructures(98);
                        p::uspool->generateProductionGroups();
                        lplayer->enableBuildAll();
                        pc::sidebar->UpdateSidebar();
			//printf ("%s line %i: update sidebar\n", __FILE__, __LINE__);
                        logger->gameMsg("Prerequisites disabled");
                    }
                    break;
                case SDLK_m:
                    if (!lplayer->hasInfMoney()) {
                        lplayer->enableInfMoney();
                        logger->gameMsg("Money check disabled");
                    }
                    break;
                case SDLK_g:
                    if (SDL_WM_GrabInput(SDL_GRAB_QUERY) == SDL_GRAB_ON) {
                        SDL_WM_GrabInput(SDL_GRAB_OFF);
                        logger->gameMsg("Mouse grab disabled");
                    } else {
                        SDL_WM_GrabInput(SDL_GRAB_ON);
                        logger->gameMsg("Mouse grab enabled");
                    }
                    break;
                default:
                    break;
                }
                break;
            }
        case SDL_KEYUP:
            if (event.key.state != SDL_RELEASED)
                break;
            {
            Uint8 k_temp = kbdmod;
            switch (event.key.keysym.sym) {
            case SDLK_RSHIFT:
            case SDLK_LSHIFT:
                k_temp &= ~k_shift;
                kbdmod = (keymod)k_temp;
                break;
            case SDLK_RCTRL:
            case SDLK_LCTRL:
                k_temp &= ~k_ctrl;
                kbdmod = (keymod)k_temp;
                break;
            case SDLK_RALT:
            case SDLK_LALT:
                k_temp &= ~k_alt;
                kbdmod = (keymod)k_temp;
                break;
            default:
                break;
            }
            }
            break;

        case SDL_ACTIVEEVENT:
            if ((event.active.state & SDL_APPACTIVE)) {
                if (event.active.gain == 1) {
                    logger->error("%s line %i: focus restored\n", __FILE__, __LINE__);

                    /* Sometimes when the app is restored the video memory is
                       lost, and then causes SDL_BlitSurface to return -2. To
                       fix this we must flush all our SDL_Surfaces and reload
                       them
                    */

                    //Clear the image cache of .shp files (units and structs)
                    pc::imgcache->flush();

                    //Reload the map tiles
                    p::ccmap->reloadTiles();

                    //Reload the sidebar
                    pc::sidebar->ReloadImages();

                    //Reload the cursor
                    pc::cursor->reloadImages();

                    //Refresh the messages
                    pc::msg->refresh();

                } else {
                    logger->error("%s line %i: focus lost\n", __FILE__, __LINE__);
                }
            }
            break;
        case SDL_QUIT:
            done = 1;
            break;
        default:
            break;
        }
    }
    sdir = CnCMap::s_none;
    keystate = SDL_GetKeyState(NULL);
    if (keystate[SDLK_LEFT])
        sdir |= CnCMap::s_left;
    else if (keystate[SDLK_RIGHT])
        sdir |= CnCMap::s_right;
    if (keystate[SDLK_UP])
        sdir |= CnCMap::s_up;
    else if (keystate[SDLK_DOWN])
        sdir |= CnCMap::s_down;
    if (sdir != CnCMap::s_none)
        p::ccmap->accScroll(sdir);

    if (keystate[SDLK_PAGEDOWN])
        pc::sidebar->ScrollSidebar(false);
    else if (keystate[SDLK_PAGEUP])
        pc::sidebar->ScrollSidebar(true);

    if (p::uspool->hasDeleted()) {
        selected->checkSelection();
    }

    if (p::ppool->pollSidebar()) {
        pc::sidebar->UpdateSidebar();
    }
    radarstat = p::ppool->statRadar();
    switch (radarstat) {
    case 0: // do nothing
        break;
    case 1: // got radar
	printf ("%s line %i: got radar\n", __FILE__, __LINE__);
	if (pc::Config.gamenum == GAME_RA) {
		pc::sfxeng->PlaySound(pc::Config.RadarUp);
	}
        pc::sidebar->StartRadarAnim(0,&minimapEnabled);
        break;
    case 2: // lost radar
    case 3: // radar powered down
	printf ("%s line %i: powerdown/lost radar\n", __FILE__, __LINE__);
        minimapEnabled = false;
        pc::sidebar->StartRadarAnim(1,&minimapEnabled);
	if (pc::Config.gamenum == GAME_RA) {
		pc::sfxeng->PlaySound(pc::Config.RadarDown);
	}
        break;
    default:
        logger->error("BUG: unexpected value returned from PlayerPool::statRadar: %i\n",
                      radarstat);
        break;
    }

    updateMousePos();

    if( p::ppool->hasWon() || p::ppool->hasLost() ) {
        ++donecount;
    }
    if (donecount == 1) {
        if (p::ppool->hasWon()) {
		pc::sfxeng->PlaySound(pc::Config.MissionWon);
		logger->gameMsg("MISSION ACCOMPLISHED");
        } else {
		pc::sfxeng->PlaySound(pc::Config.MissionLost);
		logger->gameMsg("MISSION FAILED");
        }
    }
    if (donecount > finaldelay) {
        done = 1;
    }
}

void Input::updateMousePos()
{
    /* Check the position of the mousepointer and scroll if we are
     * less than 10 pixels from an edge. */
    int mx, my;
	static int old_mx, old_my;
	static Uint32 LastTick;

    SDL_GetMouseState(&mx, &my);

	// Handle the ToolTip
	if (old_mx == mx && old_my == my){
		if (SDL_GetTicks() - LastTick > 1000){

			mx -= (width-tabwidth);
			my -= pc::sidebar->getTabLocation()->h;

			int button = pc::sidebar->getButton( mx, my );
//			printf ("Button = %i\n", button);

			if (button != 255){
				pc::sidebar->DrawButtonTooltip(button);
			}
		}
	}else{
		old_mx = mx;
		old_my = my;
		LastTick = SDL_GetTicks();
	}

    SDL_GetMouseState(&mx, &my);


    if( drawing ) { /* set cursor to the default one when drawing */
        if( mx < maparea->x )
            markrect.w = maparea->x;
        else if( mx >= maparea->x+maparea->w )
            markrect.w = maparea->x+maparea->w-1;
        else
            markrect.w = mx;
        if( my < maparea->y )
            markrect.h = my = maparea->y;
        else if( my >= maparea->y+maparea->h )
            markrect.h = maparea->y+maparea->h-1;
        else
            markrect.h = my;

        pc::cursor->setCursor(CUR_STANDARD, CUR_NOANIM);
    }
    /* Check if we should start drawing */
    else if( lmousedown == m_map && currentaction == a_none) {
        if( my >= maparea->y && my < maparea->y+maparea->h &&
                mx >= maparea->x && mx < maparea->x+maparea->w )
            if(abs(markrect.x - mx) > 2 || abs(markrect.y - my) > 2) {
                drawing = true;
                markrect.w = mx;
                markrect.h = my;
            }
    } else { /* set the correct cursor at the end of this else */
        Uint16 cursornum = 0;
        Uint8 scroll = CnCMap::s_none;

        if (( mx > width - 10 ) || (rcd_scrolling && (mx >= (sx+5))) )
            scroll |= CnCMap::s_right;
        else if (( mx < 10 ) || (rcd_scrolling && ((mx+5) <= sx)) )
            scroll |= CnCMap::s_left;
        if (( my > height - 10 ) || (rcd_scrolling && (my >= (sy+5))) )
            scroll |= CnCMap::s_down;
        else if (( my < 2 ) || (rcd_scrolling && ((my+5) <= sy)) )
            scroll |= CnCMap::s_up;

        if( scroll != CnCMap::s_none ) {
            Uint8 tmp;
            if (rcd_scrolling) {
                tmp = p::ccmap->absScroll((mx-sx),(my-sy), 5);
            } else {
                tmp = p::ccmap->accScroll(scroll);
            }
            if (tmp == CnCMap::s_none) {
                cursornum = Cursor::getNoScrollOffset();
            } else {
                scroll = tmp;
            }
            switch(scroll) {
            case CnCMap::s_up:
                cursornum += CUR_SCROLLUP;
                break;
            case CnCMap::s_upleft:
                cursornum += CUR_SCROLLUL;
                break;
            case CnCMap::s_left:
                cursornum += CUR_SCROLLLEFT;
                break;
            case CnCMap::s_downleft:
                cursornum += CUR_SCROLLDL;
                break;
            case CnCMap::s_down:
                cursornum += CUR_SCROLLDOWN;
                break;
            case CnCMap::s_downright:
                cursornum += CUR_SCROLLDR;
                break;
            case CnCMap::s_right:
                cursornum += CUR_SCROLLRIGHT;
                break;
            case CnCMap::s_upright:
                cursornum += CUR_SCROLLUR;
                break;
            default:
                cursornum = CUR_STANDARD;
            }

            if (currentaction != a_deploysuper) {
                pc::cursor->setCursor(cursornum, CUR_NOANIM);
            }
        } else if (currentaction == a_place) {
            checkPlace(mx, my);
            return;
        } else if (currentaction == a_deploysuper) {
            pc::cursor->setXY(mx, my);
            return;
        } else {
            setCursorByPos(mx, my);
        }
    }
    pc::cursor->setXY(mx, my);
}

void Input::clickMap(int mx, int my)
{
    Unit *curunit;
    Unit* tmpunit = NULL;
    Structure *curstructure;
    Uint16 pos;
    Uint8 subpos;
    bool sndplayed = false;
    bool enemy;

	clickedTile(mx, my, &pos, &subpos);
	if (pos == 0xffff){
		return;
	}

	//
	// Check if we need to heal anybody
	//
	if (selected->numbUnits() == 1){
		if (strcmp ((char*)selected->getUnit(0)->getType()->getTName(), "MEDI") == 0 && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum() && p::uspool->getUnitAt(pos) != NULL){
			if ( p::uspool->getUnitAt(pos)->getType()->isInfantry() && p::uspool->getUnitAt(pos)->getHealth() < p::uspool->getUnitAt(pos)->getType()->getMaxHealth()){
				//printf ("%s line %i: I am here\n", __FILE__, __LINE__);
				selected->getUnit(0)->attack( p::uspool->getUnitAt(pos) );
				return;
			}
		}

		if (selected->getUnit(0)->getHealth() <  selected->getUnit(0)->getType()->getMaxHealth() && p::uspool->getStructureAt(pos)!=NULL && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum()){
			if (strcmp ((char*)p::uspool->getStructureAt(pos)->getType()->getTName(), "FIX") == 0 ){
				//printf ("%s line %i: I am here\n", __FILE__, __LINE__);
				selected->getUnit(0)->Repair (p::uspool->getStructureAt(pos));
				return;
			}
		}

		// Check  if we need to harvest!!
		if (selected->getUnit(0)->IsHarvester() && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum()){
			selected->getUnit(0)->Harvest (pos, NULL);
			selected->getUnit(0)->doRandTalk(TB_ack);
			if ( p::uspool->getStructureAt(pos) != NULL){
 				if (p::uspool->getStructureAt(pos)->isRefinery ()){
					//printf ("Now try to set the base ref...\n");
					selected->getUnit(0)->SetBaseRefinery (p::uspool->getStructureAt(pos));
				}
			}
			return;
		}
	}





    switch (currentaction) {
    case a_place:
        pos = checkPlace(mx, my);
        if (pos != 0xffff) {
            currentaction = a_none;
            if (temporary_place_unit) {
                if (lplayer->isDefeated()) {
                    lplayer->setVisBuild(Player::SOB_BUILD, false);
                }
                p::dispatcher->unitCreate(placename, pos, 0, lplayer->getPlayerNum());
            } else {
                StructureType* stype = p::uspool->getStructureTypeByName(placename);
                /// @TODO Is it really OK to have input and buildqueue dealing
                // with the end result of production?
				if (p::dispatcher->structurePlace(placename, pos, lplayer->getPlayerNum())) {
					if (pc::Config.gamenum == GAME_RA) {
						//logger->warning ("%s line %i: Play some sound here??\n", __FILE__, __LINE__);
						pc::sfxeng->PlaySound("placbldg.aud");
					}else if (pc::Config.gamenum == GAME_TD) {
						pc::sfxeng->PlaySound("hvydoor1.aud");
					}
					if (!stype->isWall()) {
						if (pc::Config.gamenum == GAME_RA) {
							// We have clicked the completed structure/unit
							//logger->warning ("%s line %i: Play some sound here??\n", __FILE__, __LINE__);
							//pc::sfxeng->PlaySound(pc::Config.StructurePlaced);
							pc::sfxeng->PlaySound("build5.aud");
						}else if (pc::Config.gamenum == GAME_TD) {
							pc::sfxeng->PlaySound("constru2.aud");
						}
					}
					lplayer->placed(stype);
				}else{
					printf ("%s line %i: Failed to place structure\n", __FILE__, __LINE__);
				}
            }
        }
        return;
    default:
        break;
    }

    curunit = p::uspool->getUnitAt(pos, subpos);
    curstructure = p::uspool->getStructureAt(pos, selected->getWall());
    InfantryGroup *ig = p::uspool->getInfantryGroupAt(pos);
    if (ig && curunit == NULL) {
        curunit = ig->GetNearest(subpos);
        if (curunit->getOwner() == p::ppool->getLPlayerNum()) {
            curunit = NULL;
        }
    }


	//
	// Handle selling and repairing cursor states
	//
	if (pc::sidebar->getSpecialButtonState(1) == 2 ){
               	if (curstructure != NULL){
			if (curstructure->getOwner() == p::ppool->getLPlayerNum()){
				printf ("Repair curstructure\n");
				curstructure->repair();
				pc::sidebar->setSpecialButtonState(1, 0);
				special_but_was_down[1] = false;
				pc::sfxeng->PlaySound(pc::Config.RepairStructure);
				return;
			}
		}
	}else if (pc::sidebar->getSpecialButtonState(2) == 2 ){
               	if (curstructure != NULL){
			if (curstructure->getOwner() == p::ppool->getLPlayerNum()){
				printf ("Sell curstructure\n");
				curstructure->sell();
				pc::sidebar->setSpecialButtonState(2, 0);
				special_but_was_down[2] = false;
				pc::sfxeng->PlaySound(pc::Config.StructureSold);
				return;
			}else
				printf ("Not our structure to sell\n");
		}
	}


    if( curunit != NULL ) {
        enemy = curunit->getOwner() != p::ppool->getLPlayerNum();
        if (enemy) {
            if (selected->canAttack() && (
                        !(lplayer->isAllied(
                              p::ppool->getPlayer(curunit->getOwner()))
                         ) || kbdmod == k_ctrl) ) {
                selected->attackUnit(curunit);
                tmpunit = selected->getRandomUnit();
                if (tmpunit != NULL) {
                    //logger->debug("Play attack ack\n");
                    // Get the type (portable dynamic cast)
                    UnitType * lType = dynamic_cast<UnitType *>(tmpunit->getType());
                    //pc::sfxeng->PlaySound(((UnitType *)tmpunit->getType())->getRandTalk(TB_atkun));
                    pc::sfxeng->PlaySound(lType->getRandTalk(TB_atkun));
                }
            } else {
                selected->clearSelection();
                selected->addUnit(curunit, enemy);
            }
            return;
        } else if( kbdmod == k_ctrl) {
            if( selected->canAttack() ) {
                selected->attackUnit(curunit);
                tmpunit = selected->getRandomUnit();
                if (tmpunit != NULL) {
                    pc::sfxeng->PlaySound(((UnitType *)tmpunit->getType())->getRandTalk(TB_atkun));
                }
            }
        } else if( kbdmod == k_shift ) {
            if ( selected->isEnemy() ) {
                selected->clearSelection();
            }
            if( curunit->isSelected() )
                selected->removeUnit(curunit);
            else {
                selected->addUnit(curunit, false);
                if (!sndplayed) {
                    pc::sfxeng->PlaySound(((UnitType *)curunit->getType())->getRandTalk(TB_report));
                    sndplayed = true;
                }
            }
        } else if( !curunit->isSelected() ) {
            /*if (!enemy && !selected->empty() && !selected->isEnemy() && selected->canLoad(curunit)) {
              selected->loadUnits(curunit);
              } else { */
            selected->clearSelection();
            selected->addUnit(curunit, false);
            //}
            if (!sndplayed) {
                pc::sfxeng->PlaySound(((UnitType *)curunit->getType())->getRandTalk(TB_report));
                sndplayed = true;
            }
        } else if ((!selected->isEnemy())&&(curunit->canDeploy())) {
            selected->purge(curunit);
            selected->removeUnit(curunit);
            p::dispatcher->unitDeploy(curunit);
            pc::sidebar->UpdateSidebar();
        }
    } else if( curstructure != NULL ) {
        enemy = curstructure->getOwner() != p::ppool->getLPlayerNum();

        if( enemy ) {
            if (selected->canAttack() && (
                        !(lplayer->isAllied(
                              p::ppool->getPlayer(curstructure->getOwner()))
                         ) ||
                        kbdmod == k_ctrl) ) {
                selected->attackStructure(curstructure);
                tmpunit = selected->getRandomUnit();
                if (tmpunit != NULL) {
                    pc::sfxeng->PlaySound(((UnitType *)tmpunit->getType())->getRandTalk(TB_atkst));
                }
            } else if (!curstructure->isWall()) {
                selected->clearSelection();
                selected->addStructure(curstructure, enemy);
            }
            return;
        }
        if( kbdmod == k_ctrl ) {
            if( selected->canAttack() ) {
                selected->attackStructure(curstructure);
                tmpunit = selected->getRandomUnit();
                if (tmpunit != NULL) {
                    pc::sfxeng->PlaySound(((UnitType *)tmpunit->getType())->getRandTalk(TB_atkst));
                }
                return;
            }
        } else if( kbdmod == k_shift ) {
            if( selected->isEnemy() ) {
                selected->clearSelection();
            }
            if( curstructure->isSelected() )
                selected->removeStructure(curstructure);
            else {
                if (!curstructure->isWall())
                    selected->addStructure(curstructure, false);
            }
            return;
        } else if( kbdmod == k_alt ) {
            // hack
            curstructure->runSecAnim(5);
            return;
        } else if( !curstructure->isSelected() ) {
            /*if (!enemy && !selected->empty() && !selected->isEnemy() && selected->canLoad(curstructure)) {
              selected->loadUnits(curstructure);
              } else {*/
            selected->clearSelection();
            if (!curstructure->isWall()) {
                selected->addStructure(curstructure, false);
            }
            //}
            return;
        } else {
            if ( (curstructure != lplayer->getPrimary(curstructure->getType())) &&
                    (((StructureType*)curstructure->getType())->primarySettable()) ) {
                lplayer->setPrimary(curstructure);
                pc::sfxeng->PlaySound(pc::Config.PrimairyStructureSelected);
            }
            return;
        }
    } else {
        p::uspool->setCostCalcOwnerAndType(lplayer->getPlayerNum(),0);
		if (selected->areWaterBound()){
			if( selected->canMove() && p::ccmap->getCost(pos, selected->getRandomUnit()) < 0xfff0) {
				if (!sndplayed) {
					pc::sfxeng->PlaySound(((UnitType *)selected->getRandomUnit()->getType())->getRandTalk(TB_ack));
					sndplayed = true;
				}
				printf ("%s line %i: Move water based unit\n", __FILE__, __LINE__);
				selected->moveUnits(pos);
				new ExplosionAnim(1, pos, p::ccmap->getMoveFlashNum(),
					static_cast<Uint8>(p::ccmap->getMoveFlash()->getNumImg()), 0, 0);
			}
		}else if( selected->canMove() && p::ccmap->getCost(pos) < 0xfff0) {
            if (!sndplayed) {
                pc::sfxeng->PlaySound(((UnitType *)selected->getRandomUnit()->getType())->getRandTalk(TB_ack));
                sndplayed = true;
            }
            selected->moveUnits(pos);
            new ExplosionAnim(1, pos, p::ccmap->getMoveFlashNum(),
                    static_cast<Uint8>(p::ccmap->getMoveFlash()->getNumImg()), 0, 0);
        }
    }
}

void Input::clickedTile(int mx, int my, Uint16* pos, Uint8* subpos)
{
    Uint16 xrest, yrest, tx, ty;
    mx -= maparea->x-p::ccmap->getXTileScroll();
    my -= maparea->y-p::ccmap->getYTileScroll();
    tx = mx/tilewidth;
    ty = my/tilewidth;
    if( tx >= p::ccmap->getWidth() || ty >= p::ccmap->getHeight() )
        *pos = 0xffff;
    else {
        *pos = (my/tilewidth)*p::ccmap->getWidth()+mx/tilewidth;
        *pos += p::ccmap->getScrollPos();
    }

    xrest = mx%tilewidth;
    yrest = my%tilewidth;
    *subpos = 0;
    if( xrest >= 8 && xrest < 16 && yrest >= 8 && yrest < 16 )
        return;

    *subpos = 1;
    if( yrest >= 12 )
        (*subpos)+=2;
    if( xrest >= 12 )
        (*subpos)++;
    /* assumes the subpositions are:
     * 1 2
     *  0
     * 3 4
     */
}

void Input::setCursorByPos(int mx, int my)
{
    Uint16 pos;
    Uint8 subpos;
    Unit *curunit;
    Structure *curstruct;
    bool enemy;

    /* clicked the map */
    if( my >= maparea->y && my < maparea->y+maparea->h &&
            mx >= maparea->x && mx < maparea->x+maparea->w ) {
        clickedTile(mx, my, &pos, &subpos);
        if( pos != 0xffff ) {

		// Handle the harvesters..
		if (selected->numbUnits() == 1){
			if (selected->getUnit(0)->getHealth() <  selected->getUnit(0)->getType()->getMaxHealth() && p::uspool->getStructureAt(pos)!=NULL && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum()){
 				if (strcmp ((char*)p::uspool->getStructureAt(pos)->getType()->getTName(), "FIX") == 0 ){
					pc::cursor->setCursor("enter");
//					printf ("%s line %i: We have one unit selected and it is a harvester ;0\n", __FILE__, __LINE__);
					return;
				}
			}
			if (selected->getUnit(0)->IsHarvester() && p::ccmap->getResourceFrame(pos) != 0 && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum()){
				pc::cursor->setCursor("attack");
//				printf ("%s line %i: We have one unit selected and it is a harvester ;0\n", __FILE__, __LINE__);
				return;
			}
			if (selected->getUnit(0)->IsHarvester() && p::uspool->getStructureAt(pos) != NULL && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum()){
				if (p::uspool->getStructureAt(pos)->isRefinery ())
					pc::cursor->setCursor("enter");
					return;
			}
		}

		//
		// Handle setting the heal mouse when using the medic
		//
		if (selected->numbUnits() == 1){
			if (strcmp ((char*)selected->getUnit(0)->getType()->getTName(), "MEDI") == 0 && selected->getUnit(0)->getOwner() == p::ppool->getLPlayerNum() && p::uspool->getUnitAt(pos) != NULL){
				if ( p::uspool->getUnitAt(pos)->getType()->isInfantry() && p::uspool->getUnitAt(pos)->getHealth() < p::uspool->getUnitAt(pos)->getType()->getMaxHealth()){
					pc::cursor->setCursor("heal");
					return;
				}
			}
		}

            if( lplayer->getMapVis()[pos] ) {
                curunit = p::uspool->getUnitAt(pos, subpos);
                curstruct = p::uspool->getStructureAt(pos,selected->getWall());
                InfantryGroup *ig = p::uspool->getInfantryGroupAt(pos);
                if (ig && curunit == NULL) {
                    curunit = ig->GetNearest(subpos);
                    if (curunit->getOwner() == p::ppool->getLPlayerNum()) {
                        curunit = NULL;
                    }
                }
            } else {
                curunit = NULL;
                curstruct = NULL;
            }

		//
		// Handle selling and repairing cursor states
		//
		if (pc::sidebar->getSpecialButtonState(1) == 2 ){
			if (curstruct != NULL){
				if (curstruct->getOwner() == p::ppool->getLPlayerNum() && curstruct->getHealth() < curstruct->getType()->getMaxHealth())
        				pc::cursor->setCursor("repair");
				else
        				pc::cursor->setCursor("norepair");
			}else
        			pc::cursor->setCursor("norepair");
			return;
		}else if (pc::sidebar->getSpecialButtonState(2) == 2 ){
                	if (curstruct != NULL){
				if (curstruct->getOwner() == p::ppool->getLPlayerNum())
					pc::cursor->setCursor("sell");
				else
					pc::cursor->setCursor("nosell");
			}else
				pc::cursor->setCursor("nosell");
			return;
		}


            if( curunit != NULL ) {
                enemy = !(lplayer->isAllied(p::ppool->getPlayer(curunit->getOwner())));
                if( !curunit->isSelected() ) {
                    if( selected->canAttack() && (enemy || (kbdmod == k_ctrl))) {
                        pc::cursor->setCursor("attack");
                        return;
                    }
                    /*
                    if (!enemy && !selected->empty() && !selected->isEnemy() && selected->canLoad(curunit)) {
                    pc::cursor->setCursor("enter");
                    return;
                    }*/
                    if( selected->empty() ||
                            (!selected->empty() && !selected->isEnemy() && kbdmod == k_shift) ||
                            (!selected->canAttack() && kbdmod != k_ctrl) ) {
                        pc::cursor->setCursor("select");
                        return;
                    } else {
                        pc::cursor->setCursor("nomove");
                        return;
                    }
                } else {
                    if (curunit->getOwner() == p::ppool->getLPlayerNum()) {
                        if (((UnitType*)curunit->getType())->canDeploy()) {
                            if (curunit->canDeploy())
                                pc::cursor->setCursor("deploy");
                            else
                                pc::cursor->setCursor("nomove");
                            return;
                        } else {
                            pc::cursor->setCursor("select");
                            return;
                        }
/*
		    	if ( curunit->IsHarvester() && p::ccmap->getResourceFrame(curunit->getPos()) != 0){
				// Check the owner is the human player!!
				pc::cursor->setCursor("attack");
//				curunit->Harvest (0);
			}
*/
                    }

                }
            } else if( curstruct != NULL ) {
                enemy = !(lplayer->isAllied(p::ppool->getPlayer(curstruct->getOwner())));
                if( !curstruct->isSelected() ) {
                    if( selected->canAttack() && (enemy || (kbdmod == k_ctrl))) {
                        pc::cursor->setCursor("attack");
                        return;
                    } else if( !enemy || selected->empty() || selected->isEnemy()) {
                        if (curstruct->isWall()) {
                            pc::cursor->setCursor("nomove");
                            return;
                        } else {
                            /*if (!enemy && !selected->empty() && !selected->isEnemy() && selected->canLoad(curstruct)) {
                              pc::cursor->setCursor("enter");
                              return;
                              }*/
                            if( selected->empty() ||
                                    (!selected->empty() && !selected->isEnemy()) ||
                                    !selected->canAttack() ) {
                                pc::cursor->setCursor("select");
                                return;
                            }
                        }
                    } else if (enemy) {
                        // since we have already considerred the case
                        // when we can attack, we can't attack here.
                        if (kbdmod == k_ctrl) {
                            pc::cursor->setCursor("nomove");
                        } else {
                            pc::cursor->setCursor("select");
                        }
                        return;
                    }
                    return;
                } else {
                    if (!enemy) {
                        if ((curstruct != lplayer->getPrimary(curstruct->getType()))&&
                                (((StructureType*)curstruct->getType())->primarySettable()) ) {
                            pc::cursor->setCursor("deploy");
                            return;
                        } else {
                            pc::cursor->setCursor("select");
                            return;
                        }
                    } else {
                        pc::cursor->setCursor("select");
                        return;
                    }
                }
			}else if (selected->canMove() && selected->areWaterBound()){
				//printf ("%s line %i: Selected are water bound\n", __FILE__, __LINE__);
                p::uspool->setCostCalcOwnerAndType(lplayer->getPlayerNum(),0);
                if( p::ccmap->getCost(pos, selected->getRandomUnit()) < 0xfff0 || !lplayer->getMapVis()[pos]) {
                    pc::cursor->setCursor("move");
                } else {
                    pc::cursor->setCursor("nomove");
                }
                return;
            } else if (selected->canMove()) {
                p::uspool->setCostCalcOwnerAndType(lplayer->getPlayerNum(),0);
                if( p::ccmap->getCost(pos) < 0xfff0 || !lplayer->getMapVis()[pos]) {
                    pc::cursor->setCursor("move");
                } else {
                    pc::cursor->setCursor("nomove");
                }
                return;
            }
        }
    }

    pc::cursor->setCursor(CUR_STANDARD, CUR_NOANIM);
}

void Input::selectRegion()
{
    Uint16 startx, starty, stopx, stopy;
    Uint16 scannerx, scannery, curpos, i;
    Unit *un;
    bool playedsnd = false;

    if (kbdmod != k_shift)
        selected->clearSelection();

    startx = (min(markrect.x, (Sint16)markrect.w)-maparea->x+p::ccmap->getXTileScroll())/tilewidth+p::ccmap->getXScroll();
    starty = (min(markrect.y, (Sint16)markrect.h)-maparea->y+p::ccmap->getYTileScroll())/tilewidth+p::ccmap->getYScroll();
    stopx = (abs(markrect.x - markrect.w)+1)/tilewidth+startx;
    stopy = (abs(markrect.y - markrect.h)+1)/tilewidth+starty;
    if( stopx >= p::ccmap->getWidth() )
        stopx = p::ccmap->getWidth() - 1;
    if( stopy >= p::ccmap->getHeight() )
        stopy = p::ccmap->getHeight() - 1;

    for( scannery = starty; scannery <= stopy; scannery++ ) {
        for( scannerx = startx; scannerx <= stopx; scannerx++ ) {
            curpos = scannery*p::ccmap->getWidth()+scannerx;
            for( i = 0; i < 5; i++ ) {
                un = p::uspool->getUnitAt(curpos, static_cast<Uint8>(i));
                if( un != NULL ) {
                    if( un->getOwner() != p::ppool->getLPlayerNum() ) {
                        continue;
                    }
                    selected->addUnit(un, false);
                    if (!playedsnd) {
                        pc::sfxeng->PlaySound(((UnitType *)un->getType())->getRandTalk(TB_report));
                        playedsnd = true;
                    }
                    if( !((UnitType *)un->getType())->isInfantry() )
                        break;
                }
            }
        }
    }

}

void Input::clickSidebar(int mx, int my, bool rightbutton)
{
	Uint8		butclick;
	createmode_t	createmode;

	// Clear the selection of the player
	selected->clearSelection();

	mx -= (width-tabwidth);
	my -= pc::sidebar->getTabLocation()->h;


	butclick = pc::sidebar->getSpecialButton(mx, my);
	if (butclick != 255){
		printf ("%s line %i: Sidebar click, special button = %i\n", __FILE__, __LINE__, butclick);
		if (butclick == 1 && pc::sidebar->getSpecialButtonState(2) == 0 || butclick == 2 && pc::sidebar->getSpecialButtonState(1) == 0 || butclick == 3){
			pc::sidebar->setSpecialButtonState(butclick, true);
		}
	}


    butclick = pc::sidebar->getButton(mx, my);
    if (butclick == 255) {
        currentaction = a_none;
        return;
    }
    
    /** TODO find a more elegant way to do this, as scrolling will blank
     *  current place.
     */
    strncpy(placename, "xxxx", 4);
    pc::sidebar->ClickButton(butclick, placename, &createmode);


    if (CM_INVALID == createmode || (strncasecmp("xxxx",placename,4) == 0)) {
        currentaction = a_none;
        return;
    }

    UnitOrStructureType* type;
    if (createmode != CM_STRUCT) {
        type = p::uspool->getUnitTypeByName(placename);
    } else {
        type = p::uspool->getStructureTypeByName(placename);
    }
    ConStatus status;
    Uint8 dummy;
    // Always stopping building with a right click
    if (rightbutton) {
        status = lplayer->stopBuilding(type);
        if (BQ_EMPTY == status) {
            return;
        }

        // TODO Get these strings from a global config thiny for interop with RA
        if (BQ_PAUSED == status) {
            pc::sfxeng->PlaySound(pc::Config.BuildingOnHold);
        } else if (BQ_CANCELLED == status) {
		pc::sfxeng->PlaySound(pc::Config.BuildingCanceled);
        } else {
            logger->error("Recieved an unknown status from stopBuilding: %i\n", status);
        }
        return;
    }

    status = lplayer->getStatus(type, &dummy, &dummy);

    if ((BQ_READY == status) && (lplayer->getQueue(type->getPQueue())->getCurrentType() == type)) {
        // Clicked on finished icon
        if (createmode == CM_STRUCT) {
            // handle buildings
            placeposvalid = true;
            temporary_place_unit = false;
            currentaction = a_place;
            placetype = (StructureType*)type;
        } else {
            if (p::dispatcher->unitSpawn((UnitType*)type, lplayer->getPlayerNum())) {
                lplayer->placed(type);
            }
        }
    } 
    else 
    {
        lplayer->startBuilding(type);
        /// @TODO Check if we're building a unit and use "training" instead
        // Play buiding sound here!!!
        if (!type->isStructure()){
        	pc::sfxeng->PlaySound(pc::Config.TrainUnit);
        }
        else
        {
        	pc::sfxeng->PlaySound(pc::Config.StructureStartBuild);
        }
    }
}

Uint16 Input::checkPlace(int mx, int my)
{
    Uint16 x, y;
    Sint16 delta;
    Uint8 placexpos, placeypos;
    Uint8* placemat;
    std::vector<bool>& buildable = lplayer->getMapBuildable();
    Uint16 pos, curpos, placeoff;
    Uint8 subpos;

    // Is the cursor in the map?
    if (my < maparea->y || my >= maparea->y+maparea->h ||
            mx < maparea->x || mx >= maparea->x+maparea->w) {
        pc::cursor->setCursor(CUR_STANDARD, CUR_NOANIM);
        pc::cursor->setXY(mx, my);
        return 0xffff;
    }

    clickedTile(mx, my, &pos, &subpos);
    if (pos == 0xffff) {
        pc::cursor->setCursor(CUR_STANDARD, CUR_NOANIM);
        pc::cursor->setXY(mx, my);
        placeposvalid = false;
        return pos;
    }

    // Make sure we're placing inside the map (avoid wrapping)
    p::ccmap->translateFromPos(pos, &x, &y);
    delta = (maparea->w / tilewidth)+p::ccmap->getXScroll();
    delta -= (x + (placetype->getXsize()-1));
    if (delta <= 0) {
        x += delta-1;
        /// @BUG: Find a better way than this.
        // While working on this section, I had problems with it working only
        // when the sidebar was/wasn't visible, this seems to work around the
        // problem, but it's horrible, IMO.
        if (maparea->w % tilewidth) {
            ++x;
        }
        pos = p::ccmap->translateToPos(x,y);
    }
    delta = (maparea->h / tilewidth)+p::ccmap->getYScroll();
    delta -= (y + (placetype->getYsize()-1));
    if (delta <= 0) {
        y += delta-1;
        /// @BUG: Find a better way than this.
        // (See above for explanation)
        if (maparea->h % tilewidth) {
            ++y;
        }
       pos = p::ccmap->translateToPos(x,y);
    }

    // check if pos is valid and set cursor
    placeposvalid = true;
    /// @TODO Assumes land based buildings for now
    p::uspool->setCostCalcOwnerAndType(lplayer->getPlayerNum(),0);
    placemat = new Uint8[placetype->getXsize()*placetype->getYsize()];
    double blockedcount = 0.0;
    double rangecount = 0.0;
    for (placeypos = 0; placeypos < placetype->getYsize(); placeypos++) {
        for (placexpos = 0; placexpos < placetype->getXsize(); placexpos++) {
            curpos = pos+placeypos*p::ccmap->getWidth()+placexpos;
            placeoff = placeypos*placetype->getXsize()+placexpos;
            if (placetype->isBlocked(placeoff)) {
                ++blockedcount;
                placemat[placeoff] = 1;
				if (!p::ccmap->isBuildableAt( lplayer->getPlayerNum(), curpos, placetype->isWaterBound() )) {
					placeposvalid = false;
					// Not buildable here --> make red
					placemat[placeoff] = 4;
					continue;
				}
				if (buildable[curpos]) {
					++rangecount;
				} else {
					// Hmm inside fog or something --> make yellow
					placemat[placeoff] = 2;
				}
			} else {
				// No color??
				placemat[placeoff] = 0;
            }
        }
    }
    if (rangecount/blockedcount < getConfig().buildable_ratio) {
        placeposvalid = false;
    }
    pc::cursor->setPlaceCursor(placetype->getXsize(), placetype->getYsize(), placemat);

    delete[] placemat;
    drawing = false;

    mx = (pos%p::ccmap->getWidth() - p::ccmap->getXScroll())*tilewidth + maparea->x-p::ccmap->getXTileScroll();
    my = (pos/p::ccmap->getWidth() - p::ccmap->getYScroll())*tilewidth + maparea->y-p::ccmap->getYTileScroll();
    pc::cursor->setXY(mx, my);
    if (!placeposvalid) {
        pos = 0xffff;
    }
    return pos;
}

Uint8 Input::shouldQuit()
{
	if (done || pc::Config.quit_mission)
	{
		return true;
	} else {
		return false;
	}
}

bool Input::isMinimapEnabled()
{
    return minimapEnabled;
}

bool Input::isDrawing()
{
    return drawing;
}

SDL_Rect Input::getMarkRect()
{
    return markrect;
}
