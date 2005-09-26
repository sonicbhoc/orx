/**
 * \file orxClock.h
 * 
 * \todo
 * Add internal/external dependency system
 * Optimize dependencies storage
 * Add freezing behaviour
 */

/***************************************************************************
 begin                : 28/01/2004
 author               : (C) Arcallians
 email                : iarwain@arcallians.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _orxCLOCK_H_
#define _orxCLOCK_H_

#include "orxInclude.h"
#include "core/orxTime.h"


#define orxCLOCK_KU32_CLOCK_BANK_SIZE                 16          /**< Clock bank size */

#define orxCLOCK_KU32_FUNCTION_BANK_SIZE              16          /**< Function bank size */


/** Clock type enum */
typedef enum __orxCLOCK_TYPE_t
{
  orxCLOCK_TYPE_CORE = 0,
  orxCLOCK_TYPE_USER,

  orxCLOCK_TYPE_FPS,

  orxCLOCK_TYPE_NUMBER,

  orxCLOCK_TYPE_NONE = orxENUM_NONE

} orxCLOCK_TYPE;

/** Clock mod type enum */
typedef enum __orxCLOCK_MOD_TYPE_t
{
  orxCLOCK_MOD_TYPE_FIXED = 0,
  orxCLOCK_MOD_TYPE_MULTIPLY,

  orxCLOCK_MOD_TYPE_NUMBER,

  orxCLOCK_MOD_TYPE_NONE = orxENUM_NONE,
    
} orxCLOCK_MOD_TYPE;

/** Clock info structure. */
typedef struct __orxCLOCK_INFO_t
{
  orxCLOCK_TYPE     eType;                            /**< Clock type : 4 */
  orxU32            u32TickCounter;                   /**< Clock tick counter : 8 */
  orxU32            u32TickSize;                      /**< Clock tick size (in milliseconds) : 12 */
  orxU32            u32TickValue;                     /**< Clock current tick value (in milliseconds) ellapsed after last tick 0.0 - tick size : 16 */
  orxCLOCK_MOD_TYPE eModType;                         /**> Clock mod type : 20 */
  orxFLOAT          fModValue;                        /**> Clock mod value : 24 */
  orxU32            u32StableDT;                      /**> Clock DT (time ellapsed between 2 clock calls in milliseconds) : 28 */

  orxU32            u32Time;                          /**> Clock time (= (tick counter * tick size) + tick value : 32 */ 

} orxCLOCK_INFO;


/** Clock structure. */
typedef struct __orxCLOCK_t                           orxCLOCK;

/** Clock callback function type for used with clock bindings. */
typedef orxVOID (orxFASTCALL *orxCLOCK_FUNCTION)(orxCONST orxCLOCK_INFO *_pstClockInfo, orxVOID *_pstContext);


/** Clock module setup. */
extern orxDLLAPI orxVOID                              orxClock_Setup();
/** Inits Clock module. */
extern orxDLLAPI orxSTATUS                            orxClock_Init();
/** Exits from Clock module. */
extern orxDLLAPI orxVOID                              orxClock_Exit();

/** Updates the clock system. */
extern orxDLLAPI orxSTATUS                            orxClock_Update();

/** Creates a Clock. */
extern orxDLLAPI orxCLOCK *orxFASTCALL                orxClock_Create(orxU32 _u32TickSize, orxCLOCK_TYPE _eType);
/** Deletes a Clock. */
extern orxDLLAPI orxVOID orxFASTCALL                  orxClock_Delete(orxCLOCK *_pstClock);

/** Pauses a Clock. */
extern orxDLLAPI orxVOID orxFASTCALL                  orxClock_Pause(orxCLOCK *_pstClock);
/** Unpauses a Clock. */
extern orxDLLAPI orxVOID orxFASTCALL                  orxClock_Unpause(orxCLOCK *_pstClock);
/** Is a Clock paused? */
extern orxDLLAPI orxBOOL orxFASTCALL                  orxClock_IsPaused(orxCLOCK *_pstClock);

/** Gets informations about a Clock. */
extern orxDLLAPI orxCONST orxCLOCK_INFO *orxFASTCALL  orxClock_GetInfo(orxCONST orxCLOCK *_pstClock);

/** Registers a callback function to a clock. */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxClock_Register(orxCLOCK *_pstClock, orxCONST orxCLOCK_FUNCTION _pfnCallback, orxVOID *_pstContext, orxMODULE_ID _eModuleID);
/** Unregisters a callback function from a clock. */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxClock_Unregister(orxCLOCK *_pstClock, orxCONST orxCLOCK_FUNCTION _pfnCallback);
/** Gets a callback function context. */
extern orxDLLAPI orxVOID  *orxFASTCALL                orxClock_GetContext(orxCLOCK *_pstClock, orxCONST orxCLOCK_FUNCTION _pfnCallback);
/** Sets a callback function context. */
extern orxDLLAPI orxSTATUS orxFASTCALL                orxClock_SetContext(orxCLOCK *_pstClock, orxCONST orxCLOCK_FUNCTION _pfnCallback, orxVOID *_pstContext);

/** Finds a clock according to its tick size and its type. */
extern orxDLLAPI orxCLOCK *orxFASTCALL                orxClock_FindFirst(orxU32 _u32TickSize, orxCLOCK_TYPE _eType);
/** Finds next clock of same type/tick size. */
extern orxDLLAPI orxCLOCK *orxFASTCALL                orxClock_FindNext(orxCONST orxCLOCK *_pstClock);

/** Gets next existing clock (can be used to parse all existing clocks). */
extern orxDLLAPI orxCLOCK *orxFASTCALL                orxClock_GetNext(orxCONST orxCLOCK *_pstClock);


#endif /* _orxCLOCK_H_ */
