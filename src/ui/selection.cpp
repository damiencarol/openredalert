// Selection.cpp
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

#include "Selection.h"

#include <cassert>
#include <cstdlib>
#include <functional>
#include <map>
#include <algorithm>

#include "include/Logger.h"
#include "game/Dispatcher.h"
#include "game/Player.h"
#include "game/PlayerPool.h"
#include "game/weaponspool.h"
#include "game/UnitOrStructure.h"
#include "game/Structure.h"
#include "game/Unit.h"

namespace p {
	extern Dispatcher* dispatcher;
	extern PlayerPool* ppool;
}
extern Logger * logger;

using std::map;
using std::list;
using std::mem_fun;
using std::bind2nd;
using std::ptr_fun;
using std::unary_function;
using std::find_if;
using std::find;

/** @todo Some stuff still uses the "for (i = begin; i != end; ++i)" pattern.
 * @todo Some of the functions in the namespace can be replaced by further STL
 * magic.
 * @todo Rename function names.
 * @todo (Later) Rewrite Selection to be as close to the STL Container concept as useful
 */

namespace {

//std::mem_fun_t<void, UnitOrStructure> unref = mem_fun(&UnitOrStructure::unrefer);
//std::mem_fun_t<void, UnitOrStructure> unsel = mem_fun(&UnitOrStructure::unSelect);
//std::mem_fun_t<void, UnitOrStructure> refer = mem_fun(&UnitOrStructure::referTo);
//std::mem_fun_t<void, UnitOrStructure> select = mem_fun(&UnitOrStructure::select);

struct canAttackWalls : unary_function<UnitOrStructure*, bool> {
    bool operator()(UnitOrStructure* uos) {
        UnitOrStructureType* type = uos->getType();
        if (type->getWeapon(true) == 0) {
            return false;
        }
        return type->getWeapon(true)->getWall();
    }
};

void domove(Unit* un, Uint32 pos)
{
    p::dispatcher->unitMove(un, pos);
}

template<class T>
struct doattack : unary_function<T*, void> {
};

template<>
struct doattack<Unit> : unary_function<Unit*, void> {
    doattack(UnitOrStructure* target, bool target_is_unit) : target(target), target_is_unit(target_is_unit) {}
    void operator()(Unit* un) {
        if (un->canAttack() && (!target->getType()->isWall() || un->getType()->getWeapon(true)->getWall())) {
            p::dispatcher->unitAttack(un, target, target_is_unit);
        }
    }
    UnitOrStructure* target;
    bool target_is_unit;
};

template<>
struct doattack<Structure> : unary_function<Structure*, void> {
    doattack(UnitOrStructure* target, bool target_is_unit) : target(target), target_is_unit(target_is_unit) {}
    void operator()(Structure* st) {
        /// @todo: when power checking is done, check if structure selected
        /// can work given the player's power levels
        if (st->canAttack() && (!target->getType()->isWall() || st->getType()->getWeapon(true)->getWall())) {
            p::dispatcher->structureAttack(st, target, target_is_unit);
        }
    }
    UnitOrStructure* target;
    bool target_is_unit;
};

template<class T>
struct postloadproc : unary_function<T*, void> {
    postloadproc(Uint8 lplayernum, bool* enemysel, Uint32* numatk) :
        lplayernum(lplayernum), enemysel(*enemysel), numattacking(*numatk) {}
    void operator()(T* uos) {
        uos->select();
        bool enemy = (uos->getOwner() != lplayernum);
        if (!enemysel && enemy){
            enemysel = true;
        }
        if (!enemy && uos->canAttack()) {
            ++numattacking;
        }
    }
    Uint8 lplayernum; bool& enemysel; Uint32& numattacking;
};

void copySelection(const list<Unit*>& src_un, const list<Structure*>& src_st,
    list<Unit*>& trg_un, list<Structure*>& trg_st)
{
	list<Unit*>::iterator iter_un;
	list<Structure*>::iterator iter_st;

	for (iter_un=trg_un.begin(); iter_un != trg_un.end(); iter_un++){
		Unit *Unitp = *iter_un;
		Unitp->unrefer();
	}
    trg_un = src_un;
	for (iter_un=trg_un.begin(); iter_un != trg_un.end(); iter_un++){
		Unit *Unitp = *iter_un;
		Unitp->referTo();
	}


	for (iter_st=trg_st.begin(); iter_st != trg_st.end(); iter_st++){
		Structure *Unitp = *iter_st;
		Unitp->unrefer();
	}
    trg_st = src_st;
	for (iter_st=trg_st.begin(); iter_st != trg_st.end(); iter_st++){
		Structure *Unitp = *iter_st;
		Unitp->referTo();
	}
/*
    for_each(trg_un.begin(), trg_un.end(), (UnitOrStructure)trg_un.unref);
    trg_un = src_un;
    for_each(trg_un.begin(), trg_un.end(), (UnitOrStructure) trg_un.refer);
    for_each(trg_st.begin(), trg_st.end(), (UnitOrStructure) trg_un.unref);
    trg_st = src_st;
    for_each(trg_st.begin(), trg_st.end(), (UnitOrStructure) trg_un.refer);
*/
}

/** Remove all traces of a unit or structure in all selections*/
template<class T> void purgeImpl(T* sel, list<T*> (&saved)[10])
{
    Uint8 i;
    typename list<T*>::iterator r;

    for (i=0; i<10; ++i)
    {
        r = find(saved[i].begin(), saved[i].end(), sel);
        if (r != saved[i].end()) {
            sel->unrefer();
            saved[i].erase(r);
        }
    }
}

}

Selection::Selection() : numattacking(0), enemy_selected(false)
{
}

Selection::~Selection()
{
	list<Unit*>::iterator iter_un;
	list<Structure*>::iterator iter_st;

	for (iter_un=sel_units.begin(); iter_un != sel_units.end(); iter_un++){
		Unit *Unitp = *iter_un;
		Unitp->unrefer();
	}

	for (iter_st=sel_structs.begin(); iter_st != sel_structs.end(); iter_st++){
		Structure *Unitp = *iter_st;
		Unitp->unrefer();
	}

//    for_each(sel_units.begin(), sel_units.end(), unref);
//    for_each(sel_structs.begin(), sel_structs.end(), unref);




    for (Uint8 i=0;i<10;++i) {
//        for_each(saved_unitsel[i].begin(), saved_unitsel[i].end(), unref);
//        for_each(saved_structsel[i].begin(), saved_structsel[i].end(), unref);

		for (iter_un=saved_unitsel[i].begin(); iter_un != saved_unitsel[i].end(); iter_un++){
			Unit *Strp = *iter_un;
			Strp->unrefer();
		}

		for (iter_st=saved_structsel[i].begin(); iter_st != saved_structsel[i].end(); iter_st++){
			Structure *Strp = *iter_st;
			Strp->unrefer();
		}
    }
}

bool Selection::addUnit(Unit* selunit, bool enemy)
{
    if (enemy && enemy_selected) {
        return false;
    }
    selunit->select();
    selunit->referTo();
    sel_units.push_back(selunit);
    enemy_selected = enemy;
    if (!enemy && selunit->canAttack()) {
        ++numattacking;
    }
    return true;
}

void Selection::removeUnit(Unit *selunit)
{
//tm		*Tm;
//time_t  Now_epoch;

//	Now_epoch = time(0);
//	Tm = localtime (&Now_epoch);
//	logger->warning ("%s line %i: %02i:%02i:%02i removeUnit (Selection)\n", __FILE__, __LINE__, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

    selunit->unSelect();
    selunit->unrefer();

    sel_units.remove(selunit);

    // Can only select one enemy unit at a time, so the removed unit must have
    // been an enemy
    if (enemy_selected) {
        enemy_selected = false;
    } else if (selunit->canAttack()) {
        numattacking--;
    }
}

bool Selection::addStructure(Structure *selstruct, bool enemy)
{
    if (enemy && enemy_selected) {
        return false;
    }
    selstruct->select();
    selstruct->referTo();
    sel_structs.push_back(selstruct);
    enemy_selected = enemy;
    if (!enemy && selstruct->canAttack()) {
        ++numattacking;
    }
    return true;
}

void Selection::removeStructure(Structure *selstruct)
{
    selstruct->unSelect();
    selstruct->unrefer();

    sel_structs.remove(selstruct);

    // Can only select one enemy structure at a time, so the removed structure
    // must have been an enemy
    if (enemy_selected) {
        enemy_selected = false;
    } else if (selstruct->canAttack()) {
        --numattacking;
    }
}

void Selection::purge(Unit* sel)
{
    purgeImpl(sel, saved_unitsel);
}

void Selection::purge(Structure* sel)
{
    purgeImpl(sel, saved_structsel);
}

void Selection::clearSelection()
{
	list<Unit*>::iterator iter_un;
	list<Structure*>::iterator iter_st;

    numattacking = 0;


	for (iter_un=sel_units.begin(); iter_un != sel_units.end(); iter_un++){
		UnitOrStructure *Unitp = *iter_un;
		Unitp->unrefer();
	}
	for (iter_un=sel_units.begin(); iter_un != sel_units.end(); iter_un++){
		UnitOrStructure *Unitp = *iter_un;
		Unitp->unSelect();
	}


	for (iter_st=sel_structs.begin(); iter_st != sel_structs.end(); iter_st++){
		UnitOrStructure *Strp = *iter_st;
		Strp->unrefer();
	}

	for (iter_st=sel_structs.begin(); iter_st != sel_structs.end(); iter_st++){
		UnitOrStructure *Strp = *iter_st;
		Strp->unSelect();
	}



//    for_each(sel_units.begin(), sel_units.end(), unref);
//    for_each(sel_units.begin(), sel_units.end(), unsel);
//    for_each(sel_structs.begin(), sel_structs.end(), unref);
//    for_each(sel_structs.begin(), sel_structs.end(), unsel);
    sel_units.resize(0);
    sel_structs.resize(0);
    enemy_selected = false;
}

void Selection::moveUnits(Uint32 pos)
{
    checkSelection();
    for_each(sel_units.begin(), sel_units.end(), bind2nd(ptr_fun(domove), pos));
}

void Selection::attackUnit(Unit *target)
{
    checkSelection();
    for_each(sel_units.begin(), sel_units.end(), doattack<Unit>(target, true));
    for_each(sel_structs.begin(), sel_structs.end(), doattack<Structure>(target, true));
}

void Selection::attackStructure(Structure *target)
{
    checkSelection();
    for_each(sel_units.begin(), sel_units.end(), doattack<Unit>(target, false));
    for_each(sel_structs.begin(), sel_structs.end(), doattack<Structure>(target, false));
}

void Selection::checkSelection()
{
	tm		*Tm;
	time_t  Now_epoch;

    //remove_if(sel_units.begin(), sel_units.end(), logical_not(mem_fun(&UnitOrStructure::isAlive)));

    for (list<Unit*>::iterator it = sel_units.begin(); it != sel_units.end(); ++it) {
        assert(*it != 0);
        if (!(*it)->isAlive()) {
            Unit* unit = *it--;
            purge(unit);
            removeUnit(unit);


			Now_epoch = time(0);
			Tm = localtime (&Now_epoch);
			logger->warning ("%s line %i: %02i:%02i:%02i removeUnit (Selection)\n", __FILE__, __LINE__, Tm->tm_hour, Tm->tm_min, Tm->tm_sec);

        }
    }
    for (list<Structure*>::iterator it = sel_structs.begin(); it != sel_structs.end(); ++it) {
        if (!(*it)->isAlive()) {
            Structure* struc = *it--;
            purge(struc);
            removeStructure(struc);
        }
    }
}

Unit* Selection::getRandomUnit()
{
    Uint8 rnd,sze;
    sze = static_cast<Uint8>(sel_units.size());
    if (sze > 0) {
        rnd = (int) ((double)sze*rand()/(RAND_MAX+1.0));
        list<Unit*>::const_iterator i = sel_units.begin();
        advance(i, rnd);
        return *i;
    } else {
        return NULL;
    }
}


bool Selection::getWall() const
{
    if (sel_units.empty() && sel_structs.empty())
        return false;
    if (find_if(sel_units.begin(), sel_units.end(), canAttackWalls()) != sel_units.end()) {
        return true;
    }
    return (find_if(sel_structs.begin(), sel_structs.end(), canAttackWalls()) != sel_structs.end());
}

bool Selection::saveSelection(Uint8 savepos)
{
    if (savepos > 10)
        return false;
    if (enemy_selected)
        return false;
    copySelection(sel_units,sel_structs,saved_unitsel[savepos],saved_structsel[savepos]);
    return true;
}

bool Selection::loadSelection(Uint8 savepos)
{
    if (savepos > 10) {
        return false;
    }
    clearSelection();
    list<Unit*>& targetsel_units = saved_unitsel[savepos];
    list<Structure*>& targetsel_structs = saved_structsel[savepos];

    if (targetsel_units.empty() && targetsel_structs.empty()) {
        return false;
    }
    copySelection(targetsel_units, targetsel_structs, sel_units, sel_structs);

    Uint32 lplayernum = p::ppool->getLPlayerNum();
    for_each(sel_units.begin(), sel_units.end(), postloadproc<Unit>(lplayernum, &enemy_selected, &numattacking));
    for_each(sel_structs.begin(), sel_structs.end(), postloadproc<Structure>(lplayernum, &enemy_selected, &numattacking));

    checkSelection();
    return true;
}

void Selection::stop()
{
    for_each(sel_units.begin(), sel_units.end(), mem_fun(&Unit::stop));
    for_each(sel_structs.begin(), sel_structs.end(), mem_fun(&Structure::stop));
}

bool Selection::mergeSelection(Uint8 loadpos)
{
    if (enemy_selected) {
        return false;
    }
    list<Unit*> tmp_units; tmp_units.swap(sel_units);
    list<Structure*> tmp_structs; tmp_structs.swap(sel_structs);
    if (!loadSelection(loadpos)) {
        sel_units.swap(tmp_units);
        sel_structs.swap(tmp_structs);
        checkSelection();
        return true;
    }
    copy(tmp_units.begin(), tmp_units.end(), back_inserter(sel_units));
    copy(tmp_structs.begin(), tmp_structs.end(), back_inserter(sel_structs));
    return true;
}

Uint8 Selection::getOwner() const
{
    if (sel_units.empty()) {
        assert(!sel_structs.empty());
        return (*sel_structs.begin())->getOwner();
    }
    return (*sel_units.begin())->getOwner();
}

bool Selection::areWaterBound()
{
	UnitType	*UnitType = NULL;
	Structure	*PrimaryStruct = NULL;

	if (enemy_selected)
		return false;

    if (sel_units.empty()) {
        return false;
    }

    for (list<Unit*>::iterator UnitIt = sel_units.begin(); UnitIt != sel_units.end(); ++UnitIt) {
        assert(*UnitIt != 0);
        if (!(*UnitIt)->isAlive())
			continue;


		UnitType = (*UnitIt)->getType();
		PrimaryStruct = p::ppool->getPlayer((*UnitIt)->getOwner())->getPrimary(UnitType);
		if (PrimaryStruct != NULL){
			if (!PrimaryStruct->getType()->isWaterBound())
				return false;
		}else
			return false;
	}
	return true;
}

Uint32 Selection::numbUnits() const
{
	return sel_units.size();
}

Unit* Selection::getUnit(Uint32 UnitNumb)
{
	if (UnitNumb < sel_units.size())
	{
		list<Unit*>::const_iterator i = sel_units.begin();
		advance(i, UnitNumb);
		return *i;
	}
	return 0;
}

Structure* Selection::getStructure(Uint32 structureNumber)
{
	if (structureNumber < this->sel_structs.size())
	{
		list<Structure*>::const_iterator i = sel_structs.begin();
		advance(i, structureNumber);
		return *i;
	}
	return 0;
}

bool Selection::canAttack() const
{
	return !enemy_selected && (numattacking>0);
}

bool Selection::canMove() const
{
	return !enemy_selected && (sel_units.size() > 0);
}

bool Selection::isEnemy() const
{
	return enemy_selected;
}

bool Selection::empty() const
{
	return sel_units.empty() && sel_structs.empty();
}
