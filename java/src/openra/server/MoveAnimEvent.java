/**
 * 
 */
package openra.server;

import openra.core.CncMap;
import openra.core.Path;
import openra.core.Unit;
import openra.server.UnitAnimEvent;

/**
 * @author D.Carol
 * 
 */
public class MoveAnimEvent extends UnitAnimEvent {

	private int DefaultDelay;
	int BlockedCounter;
	boolean stopping;
	int dest;
	int newpos;
	boolean blocked;
	boolean moved_half;
	boolean pathinvalid;
	boolean waiting;
	int xmod;
	int ymod;
	int b_xmod;
	int b_ymod;
	Unit un;
	Path path;
	int istep;
	int dir;
	int range;
	
	MoveAnimEvent(int p, Unit un) {
		super(p, un);

		// logger.debug("MoveAnim cons (this %p un %p)\n",this,un);
		stopping = false;
		blocked = false;
	    range = 0;
	    this.dest = un.getTargetCell();
	    this.un = un;
	    path = null;
	    newpos = 0xffff;
	    istep = 0;
	    moved_half = true;
	    waiting = false;
	    pathinvalid = true;
	}

	void destMoveAnimEvent()
	{
		
		// TODO : OBSOLETE
	    //if (un.moveanim == this)
	    //    un.moveanim = null;
		
	    //logger.debug("MoveAnim dest (this %p un %p dest %u)\n",this,un,dest);
	   // if( !moved_half && newpos != 0xffff) {
	    //    p::uspool.abortMove(un,newpos); // Do that to released US_ flags
	   // }
		
	     path = null;
	     
	  /*  if (un.walkanim != null) {
	        un.walkanim.stop();
	    }
	    if (un.turnanim1 != null) {
	        un.turnanim1.stop();
	    }
	    if (un.turnanim2 != null) {
	        un.turnanim2.stop();
	    }*/
	}

	void stop()
	{
	    stopping = true;
	    //stopScheduled();
	}

	void run(CncMap theMap)
	{
	    int uxoff, uyoff;
	    int oldsubpos;
		int NewSubpos;

	    waiting = false;
	    if(un.getHealth() <= 0) {
	        return;
	    }

	    // TODO : Calcul du chemin
	    if (path == null) {
	    /*    p::uspool.setCostCalcOwnerAndType(un.owner, 0);
	        path = new Path(un, un.getPos(), dest, range);
	        if( !path.empty() ) {
	            startMoveOne(false);
	        } else {
	            if (un.attackanim != NULL) {
	                un.attackanim.stop();
	            }
		    //printf ("Empty path -. stop anim??\n");
	            delete this;
	        }*/
	        return;
	    }

	    if( blocked ) {
		//printf ("We are blocked\n");
	        blocked = false;
	        startMoveOne(theMap, true);
	        return;
	    }
	    /* if distance left is smaller than xmod we're ready */

	    un.xoffset += xmod;
	    un.yoffset += ymod;

	    if( !moved_half && (Math.abs(un.xoffset) >= 12 || Math.abs(un.yoffset) >= 12) ) {
	        oldsubpos = un.getSubPos();
	        NewSubpos = theMap.getUnitAndStructurePool().postMove(un, newpos);
			if (NewSubpos > 5)
			{
				System.err.println("MoveAnimEvent::run ****************** Move failed ****************");
				this.getAequeue().scheduleEvent(this);
				return;
			}
			un.setSubPos(NewSubpos);
			un.setXPos(newpos % 128);
			un.setYPos(newpos / 128);
	        un.xoffset = -un.xoffset;
	        un.yoffset = -un.yoffset;
	        if( un.getUnitType().isInfantry() ) {
	        	// TODO : CHANE THIS method
	            //un.infgrp.getSubposOffsets(oldsubpos, un.getSubPos(), uxoff, uyoff);
	            //un.xoffset += uxoff;
	            //un.yoffset += uyoff;
	            xmod = 0;
	            ymod = 0;
	            if( un.xoffset < 0 )
	                xmod = 1;
	            else if( un.xoffset > 0 )
	                xmod = -1;
	            if( un.yoffset < 0 )
	                ymod = 1;
	            else if( un.yoffset > 0 )
	                ymod = -1;
	        }
	        moved_half = true;
	    }

	    if( Math.abs(un.xoffset) < Math.abs(xmod) )
	        xmod = 0;
	    if( Math.abs(un.yoffset) < Math.abs(ymod) )
	        ymod = 0;

	    if( xmod == 0 && ymod == 0 ) {
	        un.xoffset = 0;
	        un.yoffset = 0;
	        moveDone(theMap);
		//printf ("Move done, dest = %u, ourpos = %u\n", this.dest, un.getPos());
	        return;
	    }

	    this.getAequeue().scheduleEvent(this);
	}
	
	void update()
	{
	    //logger.debug("Move updating\n");
	    dest = un.getTargetCell();
	    pathinvalid = true;
	    stopping = false;
	    range = 0;
	}

	void startMoveOne(CncMap theMap, boolean wasblocked)
	{
	    Unit BlockingUnit = null;
	    int face;
	/*#ifdef LOOPEND_TURN
	    //@todo: transport boat is jerky (?)
	    int loopend=((UnitType*)un.type).getAnimInfo().loopend;
	#endif*/
	    // TODO : Change the method
	    //newpos = p::uspool.preMove(un, path.top(), &xmod, &ymod, &BlockingUnit);

	/*#if 0
		if ( un.getOwner() == p::ccmap.getPlayerPool().getLPlayerNum() ){
			Uint16 x, y;
			p::ccmap.translateFromPos(newpos, &x, &y);
			printf ("%s line %i: Newpos = %i [%i:%i]\n", __FILE__, __LINE__, newpos, x, y);
		}
	#endif*/
	    if( newpos == 0xffff ) {
	        
	    	// TODO : Path calcul
	    	//path = null;
	        //p::uspool.setCostCalcOwnerAndType(un.owner, 0);
	        //path = new Path(un, un.getPos(), dest, range);
	        
	        pathinvalid = false;
	        
	        if( path.empty() ) {
	        	System.out.println("MoveAnimEvent.startMoveOne()");
	            System.out.println("%s line %i: path is empty\n");
	            xmod = 0;
	            ymod = 0;
	            this.getAequeue().scheduleEvent(this);
	            return;
	        }
	        // TODO : change the method !
	        //newpos = theMap.getUnitAndStructurePool().preMove(un, path.top(), &xmod, &ymod, &BlockingUnit);
	        if( newpos == 0xffff ) {
	            if (wasblocked) {
	                xmod = 0;
	                ymod = 0;
	                if (un.getAttackAnim() != null) {
	                    un.getAttackAnim().stop();
	                }
	                this.stop();
	               this.getAequeue().scheduleEvent(this);
	                return;
	            } else {
	                // @todo: tell the blocking unit to move here
	                // logger.note ("%s line %i: We are blocked\n", __FILE__, __LINE__);

	                blocked = true;

	                if (un.getWalkanim() != null) {
	                    un.getWalkanim().stop();
	                }

	                this.getAequeue().scheduleEvent(this);
	                return;
	            }
	        }
	    }

/*	#ifdef LOOPEND_TURN
	    face = ((Sint8)((loopend+1)*(8-path.top())/8))&loopend;
	#else*/
	    face = (32-(path.peek() << 2))&0x1f;

//		if (un.getOwner() == p::ccmap.getPlayerPool().getLPlayerNum())
//			printf ("%s line %i: TurnAnimEvent face(dir) = %i, path.top = %i, (path.top() << 2) = %i\n", __FILE__, __LINE__, face, path.top(), (path.top() << 2));
//	#endif
	    path.pop();

		//printf ("%s line %i: Face = %u\n", __FILE__, __LINE__, face);

	    moved_half = false;

	    if (un.getUnitType().isInfantry()) {
	        if (un.getWalkanim() != null) {
	            un.getWalkanim().changedir(face);
	        } else {
	            un.setWalkanim(new WalkAnimEvent(un.getUnitType().getSpeed(), un, face, 0));
	            this.getAequeue().scheduleEvent(un.getWalkanim());
	        }
	        this.getAequeue().scheduleEvent(this);

	    } else {
	/*#ifdef LOOPEND_TURN
	        int curface = (un.getImageNum(0)&loopend);
	        int delta = (abs(curface-face))&loopend;
//	        if( curface != face ) {
//	            if( (delta <= (Sint8)((loopend+1)/8)) || (delta >= (Sint8)(loopend*7/8))) {
			// Don't try to turn if we are already turning
	        if( curface != face && un.turnanim1 == NULL) {
	            if( ((delta <= (Sint8)((loopend+1)/8)) || (delta >= (Sint8)(loopend*7/8))) || un.getType().getType() == UN_PLANE) {
	#else*/
	        int curface = (un.getImageNum(0)&0x1f);
	        int delta = (Math.abs(curface-face))&0x1f;
	        if( (un.getImageNum(0)&0x1f) != face ) {
	            if( (delta < 5) || (delta > 27) ) {
//	#endif
					//printf ("%s line %i: Turn 1\n", __FILE__, __LINE__);
	                un.turn(face,0);
	                this.getAequeue().scheduleEvent(this);
	            } else {
					//printf ("%s line %i: Turn 2\n", __FILE__, __LINE__);
	                waiting = true;
	                un.turn(face,0);
	                un.getTurnAnim1().setSchedule(this);
	            }
	            if (un.getUnitType().getNumLayers() > 1) {
	                un.turn(face,1);
	            }
	        } else {
	        	this.getAequeue().scheduleEvent(this);
	        }
	    }
	}

	void moveDone(CncMap theMap)
	{
	    un.xoffset = 0;
	    un.yoffset = 0;

	    if (pathinvalid) {
	        
	    	// TODO : Path calcul
	    	//path = null;
	        //p.uspool.setCostCalcOwnerAndType(un.owner, 0);
	        //path = new Path(un, un.getPos(), dest, range);
	        
	        pathinvalid = false;
	    }
	    
	    if( !path.empty() && !stopping ) {
	        startMoveOne(theMap, false);
	    } else {
	        if( dest != un.getPos() && !stopping ) {
	        	// TODO : Path calcul
		    	//delete path;
	            //p.uspool.setCostCalcOwnerAndType(un.owner, 0);
	            //path = new Path(un, un.getPos(), dest, range);
	        	
	            pathinvalid = false;
	        }
	        if( path.empty() || stopping ) {
	        	// TODO : change that 
	            //delete this;
	        	
	        } else {
	            startMoveOne(theMap, false);
	        }
	    }
	}

	void setRange(int nr)
	{
		range = nr;
	}
}
