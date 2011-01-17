/* Orx - Portable Game Engine
 *
 * Copyright (c) 2008-2011 Orx-Project
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 *    1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 *    2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 *    3. This notice may not be removed or altered from any source
 *    distribution.
 */

/**
 * @file orxBody.c
 * @date 10/03/2008
 * @author iarwain@orx-project.org
 *
 */


#include "physics/orxBody.h"
#include "physics/orxPhysics.h"
#include "core/orxConfig.h"
#include "object/orxObject.h"
#include "object/orxFrame.h"
#include "utils/orxString.h"

#include "debug/orxDebug.h"
#include "memory/orxMemory.h"


/** Body flags
 */
#define orxBODY_KU32_FLAG_NONE                0x00000000  /**< No flags */

#define orxBODY_KU32_FLAG_HAS_DATA            0x00000001  /**< Has data flag */

#define orxBODY_KU32_MASK_ALL                 0xFFFFFFFF  /**< User all ID mask */


/** Module flags
 */
#define orxBODY_KU32_STATIC_FLAG_NONE         0x00000000  /**< No flags */

#define orxBODY_KU32_STATIC_FLAG_READY        0x10000000  /**< Ready flag */

#define orxBODY_KU32_MASK_ALL                 0xFFFFFFFF  /**< All mask */


/** Misc defines
 */
#define orxBODY_KZ_CONFIG_INERTIA             "Inertia"
#define orxBODY_KZ_CONFIG_MASS                "Mass"
#define orxBODY_KZ_CONFIG_LINEAR_DAMPING      "LinearDamping"
#define orxBODY_KZ_CONFIG_ANGULAR_DAMPING     "AngularDamping"
#define orxBODY_KZ_CONFIG_CUSTOM_GRAVITY      "CustomGravity"
#define orxBODY_KZ_CONFIG_FIXED_ROTATION      "FixedRotation"
#define orxBODY_KZ_CONFIG_ALLOW_SLEEP         "AllowSleep"
#define orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING "AllowGroundSliding"
#define orxBODY_KZ_CONFIG_ALLOW_MOVING        "AllowMoving"
#define orxBODY_KZ_CONFIG_HIGH_SPEED          "HighSpeed"
#define orxBODY_KZ_CONFIG_DYNAMIC             "Dynamic"
#define orxBODY_KZ_CONFIG_PART_LIST           "PartList"
#define orxBODY_KZ_CONFIG_FRICTION            "Friction"
#define orxBODY_KZ_CONFIG_RESTITUTION         "Restitution"
#define orxBODY_KZ_CONFIG_DENSITY             "Density"
#define orxBODY_KZ_CONFIG_SELF_FLAGS          "SelfFlags"
#define orxBODY_KZ_CONFIG_CHECK_MASK          "CheckMask"
#define orxBODY_KZ_CONFIG_TYPE                "Type"
#define orxBODY_KZ_CONFIG_SOLID               "Solid"
#define orxBODY_KZ_CONFIG_TOP_LEFT            "TopLeft"
#define orxBODY_KZ_CONFIG_BOTTOM_RIGHT        "BottomRight"
#define orxBODY_KZ_CONFIG_CENTER              "Center"
#define orxBODY_KZ_CONFIG_RADIUS              "Radius"
#define orxBODY_KZ_CONFIG_VERTEX_LIST         "VertexList"
#define orxBODY_KZ_CONFIG_PARENT_ANCHOR       "ParentAnchor"
#define orxBODY_KZ_CONFIG_CHILD_ANCHOR        "ChildAnchor"
#define orxBODY_KZ_CONFIG_COLLIDE             "Collide"
#define orxBODY_KZ_CONFIG_ROTATION            "Rotation"
#define orxBODY_KZ_CONFIG_MIN_ROTATION        "MinRotation"
#define orxBODY_KZ_CONFIG_MAX_ROTATION        "MaxRotation"
#define orxBODY_KZ_CONFIG_MOTOR_SPEED         "MotorSpeed"
#define orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE     "MaxMotorForce"
#define orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE    "MaxMotorTorque"
#define orxBODY_KZ_CONFIG_MAX_FORCE           "MaxForce"
#define orxBODY_KZ_CONFIG_MAX_TORQUE          "MaxTorque"
#define orxBODY_KZ_CONFIG_TRANSLATION_AXIS    "TranslationAxis"
#define orxBODY_KZ_CONFIG_MIN_TRANSLATION     "MinTranslation"
#define orxBODY_KZ_CONFIG_MAX_TRANSLATION     "MaxTranslation"
#define orxBODY_KZ_CONFIG_LENGTH              "Length"
#define orxBODY_KZ_CONFIG_FREQUENCY           "Frequency"
#define orxBODY_KZ_CONFIG_DAMPING             "Damping"
#define orxBODY_KZ_CONFIG_PARENT_GROUND_ANCHOR "ParentGroundAnchor"
#define orxBODY_KZ_CONFIG_CHILD_GROUND_ANCHOR "ChildGroundAnchor"
#define orxBODY_KZ_CONFIG_PARENT_LENGTH       "ParentLength"
#define orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH   "MaxParentLength"
#define orxBODY_KZ_CONFIG_CHILD_LENGTH        "ChildLength"
#define orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH    "MaxChildLength"
#define orxBODY_KZ_CONFIG_LENGTH_RATIO        "LengthRatio"
#define orxBODY_KZ_CONFIG_JOINT_RATIO         "JointRatio"
#define orxBODY_KZ_CONFIG_PARENT_JOINT_NAME   "ParentJoint"
#define orxBODY_KZ_CONFIG_CHILD_JOINT_NAME    "ChildJoint"

#define orxBODY_KZ_FULL                       "full"
#define orxBODY_KZ_TYPE_SPHERE                "sphere"
#define orxBODY_KZ_TYPE_BOX                   "box"
#define orxBODY_KZ_TYPE_MESH                  "mesh"
#define orxBODY_KZ_TYPE_REVOLUTE              "revolute"
#define orxBODY_KZ_TYPE_PRISMATIC             "prismatic"
#define orxBODY_KZ_TYPE_SPRING                "spring"
#define orxBODY_KZ_TYPE_ROPE                  "rope"
#define orxBODY_KZ_TYPE_PULLEY                "pulley"
#define orxBODY_KZ_TYPE_SUSPENSION            "suspension"
#define orxBODY_KZ_TYPE_WELD                  "weld"
#define orxBODY_KZ_TYPE_FRICTION              "friction"
#define orxBODY_KZ_TYPE_GEAR                  "gear"

#define orxBODY_KU32_PART_BANK_SIZE           256
#define orxBODY_KU32_JOINT_BANK_SIZE          32


/***************************************************************************
 * Structure declaration                                                   *
 ***************************************************************************/

/** Body part structure
 */
struct __orxBODY_PART_t
{
  orxLINKLIST_NODE        stNode;                                     /**< Linklist node : 12 */
  orxPHYSICS_BODY_PART   *pstData;                                    /**< Data structure : 16 */
  const orxSTRING         zReference;                                 /**< Part reference name : 20 */
};

/** Body joint structure
 */
struct __orxBODY_JOINT_t
{
  orxLINKLIST_NODE        stSrcNode;                                  /**< Linklist source node : 12 */
  orxLINKLIST_NODE        stDstNode;                                  /**< Linklist destination node : 24 */
  orxPHYSICS_BODY_JOINT  *pstData;                                    /**< Data structure : 28 */
  const orxSTRING         zReference;                                 /**< Part reference name : 32 */
};

/** Body structure
 */
struct __orxBODY_t
{
  orxSTRUCTURE            stStructure;                                /**< Public structure, first structure member : 16 */
  orxVECTOR               vScale;                                     /**< Scale : 28 */
  orxVECTOR               vPreviousPosition;                          /**< Previous position : 40 */
  orxFLOAT                fPreviousRotation;                          /**< Previous rotation : 44 */
  orxPHYSICS_BODY        *pstData;                                    /**< Physics body data : 48 */
  const orxSTRUCTURE     *pstOwner;                                   /**< Owner structure : 52 */
  orxFLOAT                fTimeMultiplier;                            /**< Current time multiplier : 56 */
  orxU32                  u32DefFlags;                                /**< Definition flags : 60 */
  orxLINKLIST             stPartList;                                 /**< Part list : 72 */
  orxLINKLIST             stSrcJointList;                             /**< Source joint list : 84 */
  orxLINKLIST             stDstJointList;                             /**< Destination joint list : 96 */
};

/** Static structure
 */
typedef struct __orxBODY_STATIC_t
{
  orxU32            u32Flags;                                         /**< Control flags */
  orxBANK          *pstPartBank;                                      /**< Part bank */
  orxBANK          *pstJointBank;                                     /**< Joint bank */

} orxBODY_STATIC;


/***************************************************************************
 * Static variables                                                        *
 ***************************************************************************/

/** Static data
 */
static orxBODY_STATIC sstBody;


/***************************************************************************
 * Private functions                                                       *
 ***************************************************************************/

/** Deletes all bodies
 */
static orxINLINE void orxBody_DeleteAll()
{
  register orxBODY *pstBody;

  /* Gets first body */
  pstBody = orxBODY(orxStructure_GetFirst(orxSTRUCTURE_ID_BODY));

  /* Non empty? */
  while(pstBody != orxNULL)
  {
    /* Deletes Body */
    orxBody_Delete(pstBody);

    /* Gets first Body */
    pstBody = orxBODY(orxStructure_GetFirst(orxSTRUCTURE_ID_BODY));
  }

  return;
}


/***************************************************************************
 * Public functions                                                        *
 ***************************************************************************/

/** Body module setup
 */
void orxFASTCALL orxBody_Setup()
{
  /* Adds module dependencies */
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_MEMORY);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_STRUCTURE);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_PHYSICS);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_FRAME);
  orxModule_AddDependency(orxMODULE_ID_BODY, orxMODULE_ID_CONFIG);

  return;
}

/** Inits the Body module
 */
orxSTATUS orxFASTCALL orxBody_Init()
{
  orxSTATUS eResult = orxSTATUS_FAILURE;

  /* Not already Initialized? */
  if((sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY) == orxBODY_KU32_STATIC_FLAG_NONE)
  {
    /* Cleans static controller */
    orxMemory_Zero(&sstBody, sizeof(orxBODY_STATIC));

    /* Creates banks */
    sstBody.pstPartBank   = orxBank_Create(orxBODY_KU32_PART_BANK_SIZE, sizeof(orxBODY_PART), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);
    sstBody.pstJointBank  = orxBank_Create(orxBODY_KU32_JOINT_BANK_SIZE, sizeof(orxBODY_JOINT), orxBANK_KU32_FLAG_NONE, orxMEMORY_TYPE_MAIN);

    /* Valid? */
    if((sstBody.pstPartBank != orxNULL)
    && (sstBody.pstJointBank != orxNULL))
    {
      /* Registers structure type */
      eResult = orxSTRUCTURE_REGISTER(BODY, orxSTRUCTURE_STORAGE_TYPE_LINKLIST, orxMEMORY_TYPE_MAIN, orxNULL);
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't create body part & body joint banks.");

      /* Deletes banks */
      if(sstBody.pstPartBank != orxNULL)
      {
        orxBank_Delete(sstBody.pstPartBank);
        sstBody.pstPartBank = orxNULL;
      }
      if(sstBody.pstJointBank != orxNULL)
      {
        orxBank_Delete(sstBody.pstJointBank);
        sstBody.pstJointBank = orxNULL;
      }

      /* Updates result */
      eResult = orxSTATUS_FAILURE;
    }
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to initialize body module when it is already initialized.");

    /* Already initialized */
    eResult = orxSTATUS_SUCCESS;
  }

  /* Initialized? */
  if(eResult != orxSTATUS_FAILURE)
  {
    /* Inits Flags */
    sstBody.u32Flags = orxBODY_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to register storage link list.");
  }

  /* Done! */
  return eResult;
}

/** Exits from the Body module
 */
void orxFASTCALL orxBody_Exit()
{
  /* Initialized? */
  if(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY)
  {
    /* Deletes body list */
    orxBody_DeleteAll();

    /* Deletes banks */
    orxBank_Delete(sstBody.pstPartBank);
    sstBody.pstPartBank = orxNULL;
    orxBank_Delete(sstBody.pstJointBank);
    sstBody.pstJointBank = orxNULL;

    /* Unregisters structure type */
    orxStructure_Unregister(orxSTRUCTURE_ID_BODY);

    /* Updates flags */
    sstBody.u32Flags &= ~orxBODY_KU32_STATIC_FLAG_READY;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Tried to exit body module when it wasn't initialized.");
  }

  return;
}

/** Creates an empty body
 * @param[in]   _pstOwner                     Body's owner used for collision callbacks (usually an orxOBJECT)
 * @param[in]   _pstBodyDef                   Body definition
 * @return      Created orxBODY / orxNULL
 */
orxBODY *orxFASTCALL orxBody_Create(const orxSTRUCTURE *_pstOwner, const orxBODY_DEF *_pstBodyDef)
{
  orxBODY    *pstBody;
  orxOBJECT  *pstObject;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(orxOBJECT(_pstOwner));
  orxASSERT(_pstBodyDef != orxNULL);

  /* Gets owner object */
  pstObject = orxOBJECT(_pstOwner);

  /* Creates body */
  pstBody = orxBODY(orxStructure_Create(orxSTRUCTURE_ID_BODY));

  /* Valid? */
  if(pstBody != orxNULL)
  {
    /* Inits flags */
    orxStructure_SetFlags(pstBody, orxBODY_KU32_FLAG_NONE, orxBODY_KU32_MASK_ALL);

    /* Creates physics body */
    pstBody->pstData = orxPhysics_CreateBody(pstBody, _pstBodyDef);

    /* Valid? */
    if(pstBody->pstData != orxNULL)
    {
      /* Stores owner */
      pstBody->pstOwner = _pstOwner;

      /* Stores its scale */
      orxObject_GetScale(pstObject, &(pstBody->vScale));

      /* Stores its definition flags */
      pstBody->u32DefFlags = _pstBodyDef->u32Flags;

      /* Clears its time multiplier */
      pstBody->fTimeMultiplier = orxFLOAT_1;

      /* Updates flags */
      orxStructure_SetFlags(pstBody, orxBODY_KU32_FLAG_HAS_DATA, orxBODY_KU32_FLAG_NONE);

      /* Increases counter */
      orxStructure_IncreaseCounter(pstBody);
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create body.");

      /* Deletes allocated structure */
      orxStructure_Delete(pstBody);
      pstBody = orxNULL;
    }
  }

  /* Done! */
  return pstBody;
}

/** Creates a body from config
 * @param[in]   _pstOwner                     Body's owner used for collision callbacks (usually an orxOBJECT)
 * @param[in]   _zConfigID                    Body config ID
 * @return      Created orxGRAPHIC / orxNULL
 */
orxBODY *orxFASTCALL orxBody_CreateFromConfig(const orxSTRUCTURE *_pstOwner, const orxSTRING _zConfigID)
{
  orxBODY *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstOwner);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxBODY_DEF stBodyDef;

    /* Clears body definition */
    orxMemory_Zero(&stBodyDef, sizeof(orxBODY_DEF));

    /* Inits it */
    stBodyDef.fInertia            = orxConfig_GetFloat(orxBODY_KZ_CONFIG_INERTIA);
    stBodyDef.fMass               = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MASS);
    stBodyDef.fLinearDamping      = orxConfig_GetFloat(orxBODY_KZ_CONFIG_LINEAR_DAMPING);
    stBodyDef.fAngularDamping     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_ANGULAR_DAMPING);
    stBodyDef.u32Flags            = orxBODY_DEF_KU32_FLAG_2D;
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_FIXED_ROTATION) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_FIXED_ROTATION;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_SLEEP) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_SLEEP) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_ALLOW_SLEEP;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_GROUND_SLIDING) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_CAN_SLIDE;
    }
    if((orxConfig_HasValue(orxBODY_KZ_CONFIG_ALLOW_MOVING) == orxFALSE) || (orxConfig_GetBool(orxBODY_KZ_CONFIG_ALLOW_MOVING) != orxFALSE))
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_CAN_MOVE;
    }
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_HIGH_SPEED) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_HIGH_SPEED;
    }
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_DYNAMIC) != orxFALSE)
    {
      stBodyDef.u32Flags |= orxBODY_DEF_KU32_FLAG_DYNAMIC;
    }

    /* Creates body */
    pstResult = orxBody_Create(_pstOwner, &stBodyDef);

    /* Valid? */
    if(pstResult != orxNULL)
    {
      orxU32 i, u32SlotCounter;

      /* Gets number of declared slots */
      u32SlotCounter = orxConfig_GetListCounter(orxBODY_KZ_CONFIG_PART_LIST);

      /* For all parts */
      for(i = 0; i < u32SlotCounter; i++)
      {
        const orxSTRING zPartName;

        /* Gets its name */
        zPartName = orxConfig_GetListString(orxBODY_KZ_CONFIG_PART_LIST, i);

        /* Valid? */
        if((zPartName != orxNULL) && (zPartName != orxSTRING_EMPTY))
        {
          /* Adds part */
          if(orxBody_AddPartFromConfig(pstResult, zPartName) == orxNULL)
          {
            /* Logs message */
            orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Couldn't add part <%s> for this body: too many parts or invalid part.", _zConfigID, orxConfig_GetListString(orxBODY_KZ_CONFIG_PART_LIST, i));
          }
        }
        else
        {
          break;
        }
      }

      /* Has custom gravity? */
      if(orxConfig_HasValue(orxBODY_KZ_CONFIG_CUSTOM_GRAVITY) != orxFALSE)
      {
        orxVECTOR vGravity;

        /* Sets it */
        orxBody_SetCustomGravity(pstResult, orxConfig_GetVector(orxBODY_KZ_CONFIG_CUSTOM_GRAVITY, &vGravity));
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Cannot find config section named (%s).", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Deletes a body
 * @param[in]   _pstBody     Body to delete
 */
orxSTATUS orxFASTCALL orxBody_Delete(orxBODY *_pstBody)
{
  orxSTATUS eResult = orxSTATUS_SUCCESS;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Decreases counter */
  orxStructure_DecreaseCounter(_pstBody);

  /* Not referenced? */
  if(orxStructure_GetRefCounter(_pstBody) == 0)
  {
    orxBODY_PART   *pstBodyPart;
    orxBODY_JOINT  *pstBodyJoint;

    /* For all parts */
    for(pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList));
        pstBodyPart != orxNULL;
        pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList)))
    {
      /* Removes it */
      orxBody_RemovePart(pstBodyPart);
    }

    /* For all source joints */
    for(pstBodyJoint = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stSrcJointList));
        pstBodyJoint != orxNULL;
        pstBodyJoint = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stSrcJointList)))
    {
      /* Removes it */
      orxBody_RemoveJoint(pstBodyJoint);
    }

    /* For all destination joints */
    for(pstBodyJoint = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stDstJointList));
        pstBodyJoint != orxNULL;
        pstBodyJoint = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stDstJointList)))
    {
      /* Removes it */
      orxBody_RemoveJoint(pstBodyJoint);
    }

    /* Has data? */
    if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
    {
      /* Deletes physics body */
      orxPhysics_DeleteBody(_pstBody->pstData);
    }

    /* Deletes structure */
    orxStructure_Delete(_pstBody);
  }
  else
  {
    /* Referenced by others */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Gets a body owner
 * @param[in]   _pstBody        Concerned body
 * @return      orxSTRUCTURE / orxNULL
 */
orxSTRUCTURE *orxFASTCALL orxBody_GetOwner(const orxBODY *_pstBody)
{
  orxSTRUCTURE *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Updates result */
  pstResult = orxSTRUCTURE(_pstBody->pstOwner);

  /* Done! */
  return pstResult;
}

/** Adds a part to body
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyPartDef Body part definition
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_AddPart(orxBODY *_pstBody, const orxBODY_PART_DEF *_pstBodyPartDef)
{
  orxBODY_PART *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pstBodyPartDef != orxNULL);

  /* Creates a body part */
  pstResult = (orxBODY_PART *)orxBank_Allocate(sstBody.pstPartBank);

  /* Valid? */
  if(pstResult != orxNULL)
  {
    orxPHYSICS_BODY_PART *pstBodyPart;

    /* Clears it */
    orxMemory_Zero(pstResult, sizeof(orxBODY_PART));

    /* Creates physics part */
    pstBodyPart = orxPhysics_CreateBodyPart(_pstBody->pstData, pstResult, _pstBodyPartDef);

    /* Valid? */
    if(pstBodyPart != orxNULL)
    {
      /* Stores its data */
      pstResult->pstData = pstBodyPart;

      /* Links it */
      orxLinkList_AddEnd(&(_pstBody->stPartList), &(pstResult->stNode));
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create body part.");

      /* Deletes part */
      orxBank_Free(sstBody.pstPartBank, pstResult);

      /* Updates result */
      pstResult = orxNULL;
    }
  }

  /* Done! */
  return pstResult;
}

/** Adds a part to body from config
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _zConfigID      Body part config ID
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_AddPartFromConfig(orxBODY *_pstBody, const orxSTRING _zConfigID)
{
  orxBODY_PART *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxSTRING         zBodyPartType;
    orxBODY_PART_DEF  stBodyPartDef;
    orxBOOL           bSuccess = orxTRUE;

    /* Clears body part definition */
    orxMemory_Zero(&stBodyPartDef, sizeof(orxBODY_PART_DEF));

    /* Gets body part type */
    zBodyPartType = orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_TYPE));

    /* Inits it */
    stBodyPartDef.fFriction     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_FRICTION);
    stBodyPartDef.fRestitution  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_RESTITUTION);
    stBodyPartDef.fDensity      = orxConfig_GetFloat(orxBODY_KZ_CONFIG_DENSITY);
    stBodyPartDef.u16SelfFlags  = (orxU16)orxConfig_GetU32(orxBODY_KZ_CONFIG_SELF_FLAGS);
    stBodyPartDef.u16CheckMask  = (orxU16)orxConfig_GetU32(orxBODY_KZ_CONFIG_CHECK_MASK);
    orxVector_Copy(&(stBodyPartDef.vScale), &(_pstBody->vScale));
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_SOLID) != orxFALSE)
    {
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_SOLID;
    }
    /* Sphere? */
    if(orxString_Compare(zBodyPartType, orxBODY_KZ_TYPE_SPHERE) == 0)
    {
      /* Updates sphere specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_SPHERE;
      if(((orxConfig_HasValue(orxBODY_KZ_CONFIG_CENTER) == orxFALSE)
       && (orxConfig_HasValue(orxBODY_KZ_CONFIG_RADIUS) == orxFALSE))
      || (orxString_Compare(orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_RADIUS)), orxBODY_KZ_FULL) == 0)
      || (orxString_Compare(orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_CENTER)), orxBODY_KZ_FULL) == 0))
      {
        orxVECTOR vPivot, vSize;

        /* Gets object size & pivot */
        orxObject_GetSize(orxOBJECT(_pstBody->pstOwner), &vSize);
        orxObject_GetPivot(orxOBJECT(_pstBody->pstOwner), &vPivot);

        /* Gets radius size */
        orxVector_Mulf(&vSize, &vSize, orx2F(0.5f));

        /* Inits body part def */
        orxVector_Set(&(stBodyPartDef.stSphere.vCenter), vSize.fX - vPivot.fX, vSize.fY - vPivot.fY, vSize.fZ - vPivot.fZ);
        stBodyPartDef.stSphere.fRadius = orxMAX(vSize.fX, orxMAX(vSize.fY, vSize.fZ));
      }
      else
      {
        orxConfig_GetVector(orxBODY_KZ_CONFIG_CENTER, &(stBodyPartDef.stSphere.vCenter));
        stBodyPartDef.stSphere.fRadius = orxConfig_GetFloat(orxBODY_KZ_CONFIG_RADIUS);
      }
    }
    /* Box? */
    else if(orxString_Compare(zBodyPartType, orxBODY_KZ_TYPE_BOX) == 0)
    {
      /* Updates box specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_BOX;
      if(((orxConfig_HasValue(orxBODY_KZ_CONFIG_TOP_LEFT) == orxFALSE)
       && (orxConfig_HasValue(orxBODY_KZ_CONFIG_BOTTOM_RIGHT) == orxFALSE))
      || (orxString_Compare(orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_TOP_LEFT)), orxBODY_KZ_FULL) == 0)
      || (orxString_Compare(orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_BOTTOM_RIGHT)), orxBODY_KZ_FULL) == 0))
      {
        orxVECTOR vPivot, vSize;

        /* Gets object size & pivot */
        orxObject_GetSize(orxOBJECT(_pstBody->pstOwner), &vSize);
        orxObject_GetPivot(orxOBJECT(_pstBody->pstOwner), &vPivot);

        /* Inits body part def */
        orxVector_Set(&(stBodyPartDef.stAABox.stBox.vTL), -vPivot.fX, -vPivot.fY, -vPivot.fZ);
        orxVector_Set(&(stBodyPartDef.stAABox.stBox.vBR), vSize.fX - vPivot.fX, vSize.fY - vPivot.fY, vSize.fZ - vPivot.fZ);
      }
      else
      {
        orxConfig_GetVector(orxBODY_KZ_CONFIG_TOP_LEFT, &(stBodyPartDef.stAABox.stBox.vTL));
        orxConfig_GetVector(orxBODY_KZ_CONFIG_BOTTOM_RIGHT, &(stBodyPartDef.stAABox.stBox.vBR));
      }
    }
    /* Mesh */
    else if(orxString_Compare(zBodyPartType, orxBODY_KZ_TYPE_MESH) == 0)
    {
      /* Updates mesh specific info */
      stBodyPartDef.u32Flags |= orxBODY_PART_DEF_KU32_FLAG_MESH;
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_VERTEX_LIST) != orxFALSE)
      && ((stBodyPartDef.stMesh.u32VertexCounter = orxConfig_GetListCounter(orxBODY_KZ_CONFIG_VERTEX_LIST)) >= 3))
      {
        orxU32 i;

        /* Too many defined vertices? */
        if(stBodyPartDef.stMesh.u32VertexCounter > orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER)
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Too many vertices in the list: %ld. The maximum allowed is: %ld. Using the first %ld ones for the shape <%s>", stBodyPartDef.stMesh.u32VertexCounter, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER, _zConfigID);

          /* Updates vertices number */
          stBodyPartDef.stMesh.u32VertexCounter = orxBODY_PART_DEF_KU32_MESH_VERTEX_NUMBER;
        }

        /* For all defined vertices */
        for(i = 0; i < stBodyPartDef.stMesh.u32VertexCounter; i++)
        {
          /* Gets its vector */
          orxConfig_GetListVector(orxBODY_KZ_CONFIG_VERTEX_LIST, i, &(stBodyPartDef.stMesh.avVertices[i]));
        }
      }
      else
      {
        /* Logs message */
        orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Vertex list for creating mesh body <%s> is invalid (missing or less than 3 vertices).", _zConfigID);

        /* Updates status */
        bSuccess = orxFALSE;
      }
    }
    /* Unknown */
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "<%s> isn't a valid type for a body part.", zBodyPartType);

      /* Updates status */
      bSuccess = orxFALSE;
    }

    /* Valid? */
    if(bSuccess != orxFALSE)
    {
      /* Adds body part */
      pstResult = orxBody_AddPart(_pstBody, &stBodyPartDef);

      /* Valid? */
      if(pstResult != orxNULL)
      {
        /* Stores its reference */
        pstResult->zReference = orxConfig_GetCurrentSection();

        /* Protects it */
        orxConfig_ProtectSection(pstResult->zReference, orxTRUE);
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't find config section named (%s)", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Gets next body part
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyPart    Current body part (orxNULL to get the first one)
 * @return      orxBODY_PART / orxNULL
 */
orxBODY_PART *orxFASTCALL orxBody_GetNextPart(const orxBODY *_pstBody, const orxBODY_PART *_pstBodyPart)
{
  orxBODY_PART *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* First one? */
  if(_pstBodyPart == orxNULL)
  {
    /* Updates result */
    pstResult = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList));
  }
  else
  {
    /* Updates result */
    pstResult = (orxBODY_PART *)orxLinkList_GetNext(&(_pstBodyPart->stNode));
  }

  /* Done! */
  return pstResult;
}

/** Gets a body part name
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxSTRING / orxNULL
 */
const orxSTRING orxFASTCALL orxBody_GetPartName(const orxBODY_PART *_pstBodyPart)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyPart != orxNULL);

  /* Updates result */
  zResult = _pstBodyPart->zReference;

  /* Done! */
  return zResult;
}

/** Removes a body part
 * @param[in]   _pstBodyPart    Concerned body part
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_RemovePart(orxBODY_PART *_pstBodyPart)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyPart != orxNULL)
  {
    /* Unlinks it */
    orxLinkList_Remove(&(_pstBodyPart->stNode));

    /* Deletes its data */
    orxPhysics_DeleteBodyPart(_pstBodyPart->pstData);

    /* Has reference? */
    if(_pstBodyPart->zReference != orxNULL)
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstBodyPart->zReference, orxFALSE);
    }

    /* Frees part */
    orxBank_Free(sstBody.pstPartBank, _pstBodyPart);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Adds a joint to link two bodies together
 * @param[in]   _pstSrcBody       Concerned source body
 * @param[in]   _pstDstBody       Concerned destination body
 * @param[in]   _pstBodyJointDef  Body joint definition
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_AddJoint(orxBODY *_pstSrcBody, orxBODY *_pstDstBody, const orxBODY_JOINT_DEF *_pstBodyJointDef)
{
  orxBODY_JOINT *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSrcBody);
  orxSTRUCTURE_ASSERT(_pstDstBody);
  orxASSERT(_pstBodyJointDef != orxNULL);

  /* Creates a body part */
  pstResult = (orxBODY_JOINT *)orxBank_Allocate(sstBody.pstJointBank);

  /* Valid? */
  if(pstResult != orxNULL)
  {
    orxPHYSICS_BODY_JOINT *pstBodyJoint;

    /* Clears it */
    orxMemory_Zero(pstResult, sizeof(orxBODY_JOINT));

    /* Creates physics joint */
    pstBodyJoint = orxPhysics_CreateBodyJoint(_pstSrcBody->pstData, _pstDstBody->pstData, pstResult, _pstBodyJointDef);

    /* Valid? */
    if(pstBodyJoint != orxNULL)
    {
      /* Stores its data */
      pstResult->pstData = pstBodyJoint;

      /* Links it */
      orxLinkList_AddEnd(&(_pstSrcBody->stSrcJointList), &(pstResult->stSrcNode));
      orxLinkList_AddEnd(&(_pstDstBody->stDstJointList), &(pstResult->stDstNode));
    }
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Failed to create body part.");

      /* Deletes part */
      orxBank_Free(sstBody.pstPartBank, pstResult);

      /* Updates result */
      pstResult = orxNULL;
    }
  }

  /* Done! */
  return pstResult;
}

/** Adds a joint from config to link two bodies together
 * @param[in]   _pstSrcBody     Concerned source body
 * @param[in]   _pstDstBody     Concerned destination body
 * @param[in]   _zConfigID      Body joint config ID
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_AddJointFromConfig(orxBODY *_pstSrcBody, orxBODY *_pstDstBody, const orxSTRING _zConfigID)
{
  orxBODY_JOINT *pstResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstSrcBody);
  orxSTRUCTURE_ASSERT(_pstDstBody);
  orxASSERT((_zConfigID != orxNULL) && (_zConfigID != orxSTRING_EMPTY));

  /* Pushes section */
  if((orxConfig_HasSection(_zConfigID) != orxFALSE)
  && (orxConfig_PushSection(_zConfigID) != orxSTATUS_FAILURE))
  {
    orxSTRING         zBodyJointType;
    orxBODY_JOINT_DEF stBodyJointDef;
    orxBOOL           bSuccess = orxTRUE;

    /* Clears body part definition */
    orxMemory_Zero(&stBodyJointDef, sizeof(orxBODY_JOINT_DEF));

    /* Gets body joint type */
    zBodyJointType = orxString_LowerCase((orxSTRING)orxConfig_GetString(orxBODY_KZ_CONFIG_TYPE));

    /* Inits it */
    orxConfig_GetVector(orxBODY_KZ_CONFIG_PARENT_ANCHOR, &(stBodyJointDef.vSrcAnchor));
    orxConfig_GetVector(orxBODY_KZ_CONFIG_CHILD_ANCHOR, &(stBodyJointDef.vDstAnchor));
    if(orxConfig_GetBool(orxBODY_KZ_CONFIG_COLLIDE) != orxFALSE)
    {
      stBodyJointDef.u32Flags = orxBODY_JOINT_DEF_KU32_FLAG_COLLIDE;
    }

    /* Revolute? */
    if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_REVOLUTE) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_REVOLUTE;

      /* Stores default rotation */
      stBodyJointDef.stRevolute.fDefaultRotation  = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));

      /* Has rotation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_ROTATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_ROTATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags                  |= orxBODY_JOINT_DEF_KU32_FLAG_ROTATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stRevolute.fMinRotation    = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_ROTATION);
        stBodyJointDef.stRevolute.fMaxRotation    = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_ROTATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stRevolute.fMotorSpeed     = orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stRevolute.fMaxMotorTorque = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_TORQUE);

        /* Updates status */
        stBodyJointDef.u32Flags                  |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Prismatic? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_PRISMATIC) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_PRISMATIC;

      /* Stores default rotation */
      stBodyJointDef.stPrismatic.fDefaultRotation = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));

      /* Stores translation axis */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_TRANSLATION_AXIS, &(stBodyJointDef.stPrismatic.vTranslationAxis));

      /* Has translation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_TRANSLATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_TRANSLATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_TRANSLATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stPrismatic.fMinTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_TRANSLATION);
        stBodyJointDef.stPrismatic.fMaxTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TRANSLATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stPrismatic.fMotorSpeed      = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stPrismatic.fMaxMotorForce   = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE);

        /* Updates status */
        stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Spring? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_SPRING) == 0)
    {
      orxVECTOR vSrcPos, vDstPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_SPRING;

      /* Stores length */
      stBodyJointDef.stSpring.fLength     = orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH) : orxVector_GetDistance(orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vSrcPos), orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vDstPos));;

      /* Stores frequency */
      stBodyJointDef.stSpring.fFrequency  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_FREQUENCY);

      /* Stores damping */
      stBodyJointDef.stSpring.fDamping    = orxConfig_GetFloat(orxBODY_KZ_CONFIG_DAMPING);
    }
    /* Rope? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_ROPE) == 0)
    {
      orxVECTOR vSrcPos, vDstPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_ROPE;

      /* Stores length */
      stBodyJointDef.stRope.fLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH) : orxVector_GetDistance(orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vSrcPos), orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vDstPos));;
    }
    /* Pulley? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_PULLEY) == 0)
    {
      orxVECTOR vPos;

      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_PULLEY;

      /* Stores ratio */
      stBodyJointDef.stPulley.fLengthRatio  = (orxConfig_HasValue(orxBODY_KZ_CONFIG_LENGTH_RATIO) != orxFALSE) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_LENGTH_RATIO) : orxFLOAT_1;

      /* Stores anchors */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_PARENT_GROUND_ANCHOR, &(stBodyJointDef.stPulley.vSrcGroundAnchor));
      orxConfig_GetVector(orxBODY_KZ_CONFIG_CHILD_GROUND_ANCHOR, &(stBodyJointDef.stPulley.vDstGroundAnchor));

      /* Stores lengths */
      stBodyJointDef.stPulley.fSrcLength    = orxConfig_HasValue(orxBODY_KZ_CONFIG_PARENT_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_PARENT_LENGTH) : orxVector_GetDistance(orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstSrcBody)), &vPos), &(stBodyJointDef.stPulley.vSrcGroundAnchor));
      stBodyJointDef.stPulley.fDstLength    = orxConfig_HasValue(orxBODY_KZ_CONFIG_CHILD_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_CHILD_LENGTH) : orxVector_GetDistance(orxObject_GetPosition(orxOBJECT(orxBody_GetOwner(_pstDstBody)), &vPos), &(stBodyJointDef.stPulley.vDstGroundAnchor));
      stBodyJointDef.stPulley.fMaxSrcLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_PARENT_LENGTH) : stBodyJointDef.stPulley.fSrcLength + stBodyJointDef.stPulley.fLengthRatio * stBodyJointDef.stPulley.fDstLength;
      stBodyJointDef.stPulley.fMaxDstLength = orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_CHILD_LENGTH) : stBodyJointDef.stPulley.fSrcLength + stBodyJointDef.stPulley.fDstLength;
    }
    /* Suspension? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_SUSPENSION) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_SUSPENSION;

      /* Stores translation axis */
      orxConfig_GetVector(orxBODY_KZ_CONFIG_TRANSLATION_AXIS, &(stBodyJointDef.stSuspension.vTranslationAxis));

      /* Has translation limits? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MIN_TRANSLATION) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_TRANSLATION) != orxFALSE))
      {
        /* Updates status */
        stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_TRANSLATION_LIMIT;

        /* Stores them */
        stBodyJointDef.stSuspension.fMinTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MIN_TRANSLATION);
        stBodyJointDef.stSuspension.fMaxTranslation  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TRANSLATION);
      }

      /* Is a motor? */
      if((orxConfig_HasValue(orxBODY_KZ_CONFIG_MOTOR_SPEED) != orxFALSE)
      && (orxConfig_HasValue(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE) != orxFALSE))
      {
        /* Stores motor values */
        stBodyJointDef.stSuspension.fMotorSpeed     = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MOTOR_SPEED);
        stBodyJointDef.stSuspension.fMaxMotorForce  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_MOTOR_FORCE);

        /* Updates status */
        stBodyJointDef.u32Flags                    |= orxBODY_JOINT_DEF_KU32_FLAG_MOTOR;
      }
    }
    /* Weld? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_WELD) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags                |= orxBODY_JOINT_DEF_KU32_FLAG_WELD;

      /* Stores default rotation */
      stBodyJointDef.stWeld.fDefaultRotation  = orxConfig_HasValue(orxBODY_KZ_CONFIG_ROTATION) ? orxMATH_KF_DEG_TO_RAD * orxConfig_GetFloat(orxBODY_KZ_CONFIG_ROTATION) : orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstDstBody))) - orxObject_GetRotation(orxOBJECT(orxBody_GetOwner(_pstSrcBody)));
    }
    /* Friction? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_FRICTION) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_FRICTION;

      /* Stores max force & torque values */
      stBodyJointDef.stFriction.fMaxForce   = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_FORCE);
      stBodyJointDef.stFriction.fMaxTorque  = orxConfig_GetFloat(orxBODY_KZ_CONFIG_MAX_TORQUE);
    }
    /* Gear? */
    else if(orxString_Compare(zBodyJointType, orxBODY_KZ_TYPE_GEAR) == 0)
    {
      /* Stores type */
      stBodyJointDef.u32Flags |= orxBODY_JOINT_DEF_KU32_FLAG_GEAR;

      /* Stores joint names */
      stBodyJointDef.stGear.zSrcJointName = orxConfig_GetString(orxBODY_KZ_CONFIG_PARENT_JOINT_NAME);
      stBodyJointDef.stGear.zDstJointName = orxConfig_GetString(orxBODY_KZ_CONFIG_CHILD_JOINT_NAME);

      /* Stores joint ratio */
      stBodyJointDef.stGear.fJointRatio   = (orxConfig_HasValue(orxBODY_KZ_CONFIG_JOINT_RATIO) != orxFALSE) ? orxConfig_GetFloat(orxBODY_KZ_CONFIG_JOINT_RATIO) : orxFLOAT_1;
    }
    /* Unknown */
    else
    {
      /* Logs message */
      orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Can't create body joint: <%s> isn't a valid type.", zBodyJointType);

      /* Updates status */
      bSuccess = orxFALSE;
    }

    /* Valid? */
    if(bSuccess != orxFALSE)
    {
      /* Adds body part */
      pstResult = orxBody_AddJoint(_pstSrcBody, _pstDstBody, &stBodyJointDef);

      /* Valid? */
      if(pstResult != orxNULL)
      {
        /* Stores its reference */
        pstResult->zReference = orxConfig_GetCurrentSection();

        /* Protects it */
        orxConfig_ProtectSection(pstResult->zReference, orxTRUE);
      }
    }

    /* Pops previous section */
    orxConfig_PopSection();
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Couldn't find config section named (%s)", _zConfigID);

    /* Updates result */
    pstResult = orxNULL;
  }

  /* Done! */
  return pstResult;
}

/** Gets next body joint
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pstBodyJoint   Current body joint (orxNULL to get the first one)
 * @return      orxBODY_JOINT / orxNULL
 */
orxBODY_JOINT *orxFASTCALL orxBody_GetNextJoint(const orxBODY *_pstBody, const orxBODY_JOINT *_pstBodyJoint)
{
  orxBODY_JOINT *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* First one? */
  if(_pstBodyJoint == orxNULL)
  {
    /* Gets first source joint */
    pstResult = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stSrcJointList));

    /* Invalid? */
    if(pstResult == orxNULL)
    {
      /* Gets first destination joint */
      pstResult = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stDstJointList));
    }
  }
  else
  {
    /* Is a source joint? */
    if(_pstBodyJoint->stSrcNode.pstList == &(_pstBody->stSrcJointList))
    {
      /* Is last? */
      if(&(_pstBodyJoint->stSrcNode) == orxLinkList_GetLast(&(_pstBody->stSrcJointList)))
      {
        /* Gets first destination joint */
        pstResult = (orxBODY_JOINT *)orxLinkList_GetFirst(&(_pstBody->stDstJointList));
      }
    }
    else
    {
      /* Checks */
      orxASSERT(_pstBodyJoint->stDstNode.pstList == &(_pstBody->stDstJointList));

      /* Gets next destination joint */
      pstResult = (orxBODY_JOINT *)((orxU8 *)orxLinkList_GetNext(&(_pstBodyJoint->stDstNode)) - (orxU8 *)&(((orxBODY_JOINT *)0)->stDstNode));
    }
  }

  /* Done! */
  return pstResult;
}

/** Gets a body joint name
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @return      orxSTRING / orxNULL
 */
const orxSTRING orxFASTCALL orxBody_GetJointName(const orxBODY_JOINT *_pstBodyJoint)
{
  const orxSTRING zResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pstBodyJoint != orxNULL);

  /* Updates result */
  zResult = _pstBodyJoint->zReference;

  /* Done! */
  return zResult;
}

/** Removes a body joint
 * @param[in]   _pstBodyJoint   Concerned body joint
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_RemoveJoint(orxBODY_JOINT *_pstBodyJoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);

  /* Valid? */
  if(_pstBodyJoint != orxNULL)
  {
    /* Unlinks it */
    orxLinkList_Remove(&(_pstBodyJoint->stSrcNode));
    orxLinkList_Remove(&(_pstBodyJoint->stDstNode));

    /* Deletes its data */
    orxPhysics_DeleteBodyJoint(_pstBodyJoint->pstData);

    /* Has reference? */
    if(_pstBodyJoint->zReference != orxNULL)
    {
      /* Unprotects it */
      orxConfig_ProtectSection(_pstBodyJoint->zReference, orxFALSE);
    }

    /* Frees joint */
    orxBank_Free(sstBody.pstJointBank, _pstBodyJoint);

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body position
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvPosition     Position to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetPosition(orxBODY *_pstBody, const orxVECTOR *_pvPosition)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvPosition != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetPosition(_pstBody->pstData, _pvPosition);
    orxVector_Copy(&(_pstBody->vPreviousPosition), _pvPosition);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body rotation
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fRotation      Rotation to set (radians)
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetRotation(orxBODY *_pstBody, orxFLOAT _fRotation)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetRotation(_pstBody->pstData, _fRotation);
    _pstBody->fPreviousRotation = _fRotation;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body scale
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvScale        Scale to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetScale(orxBODY *_pstBody, const orxVECTOR *_pvScale)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvScale != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Is new scale different? */
    if(orxVector_AreEqual(_pvScale, &(_pstBody->vScale)) == orxFALSE)
    {
      orxBODY_PART *pstBodyPart;
      orxU32        u32Counter;

      /* Stores it */
      orxVector_Copy(&(_pstBody->vScale), _pvScale);

      /* For all parts */
      for(u32Counter = orxLinkList_GetCounter(&(_pstBody->stPartList)), pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList));
          u32Counter > 0;
          u32Counter--, pstBodyPart = (orxBODY_PART *)orxLinkList_GetFirst(&(_pstBody->stPartList)))
      {
        /* Has reference? */
        if(pstBodyPart->zReference != orxNULL)
        {
          const orxSTRING zReference;

          /* Stores it locally */
          zReference = pstBodyPart->zReference;

          /* Removes part */
          orxBody_RemovePart(pstBodyPart);

          /* Creates new part */
          orxBody_AddPartFromConfig(_pstBody, zReference);
        }
        else
        {
          /* Logs message */
          orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "[%s]: Scaling of body part with no config reference is unsupported. Please scale only bodies that contain parts created from config.", (_pstBody->pstOwner != orxNULL) ? orxObject_GetName(orxOBJECT(_pstBody->pstOwner)) : "UNDEFINED");
        }
      }

      //! TODO: For all joints
    }

    /* Updates result */
    eResult = orxSTATUS_SUCCESS;
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body speed
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvSpeed        Speed to set
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetSpeed(orxBODY *_pstBody, const orxVECTOR *_pvSpeed)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvSpeed != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its speed */
    eResult = orxPhysics_SetSpeed(_pstBody->pstData, _pvSpeed);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Strcuture does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body angular velocity
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fVelocity      Angular velocity to set (radians/seconds)
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetAngularVelocity(orxBODY *_pstBody, orxFLOAT _fVelocity)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetAngularVelocity(_pstBody->pstData, _fVelocity);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Sets a body custom gravity
 * @param[in]   _pstBody          Concerned body
 * @param[in]   _pvCustomGravity  Custom gravity to set / orxNULL to remove it
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_SetCustomGravity(orxBODY *_pstBody, const orxVECTOR *_pvCustomGravity)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates its position */
    eResult = orxPhysics_SetCustomGravity(_pstBody->pstData, _pvCustomGravity);
  }
  else
  {
    /* Logs message */
    orxDEBUG_PRINT(orxDEBUG_LEVEL_PHYSICS, "Structure does not have data.");

    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Gets a body position
 * @param[in]   _pstBody        Concerned body
 * @param[out]  _pvPosition     Position to get
 * @return      Body position / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetPosition(const orxBODY *_pstBody, orxVECTOR *_pvPosition)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvPosition != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    pvResult = orxPhysics_GetPosition(_pstBody->pstData, _pvPosition);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets a body rotation
 * @param[in]   _pstBody        Concerned body
 * @return      Body rotation (radians)
 */
orxFLOAT orxFASTCALL orxBody_GetRotation(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    fResult = orxPhysics_GetRotation(_pstBody->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets a body speed
 * @param[in]   _pstBody        Concerned body
 * @param[out]   _pvSpeed       Speed to get
 * @return      Body speed / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetSpeed(const orxBODY *_pstBody, orxVECTOR *_pvSpeed)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvSpeed != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    pvResult = orxPhysics_GetSpeed(_pstBody->pstData, _pvSpeed);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets a body angular velocity
 * @param[in]   _pstBody        Concerned body
 * @return      Body angular velocity (radians/seconds)
 */
orxFLOAT orxFASTCALL orxBody_GetAngularVelocity(const orxBODY *_pstBody)
{
  orxFLOAT fResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    fResult = orxPhysics_GetAngularVelocity(_pstBody->pstData);
  }
  else
  {
    /* Updates result */
    fResult = orxFLOAT_0;
  }

  /* Done! */
  return fResult;
}

/** Gets a body custom gravity
 * @param[in]   _pstBody          Concerned body
 * @param[out]  _pvCustomGravity  Custom gravity to get
 * @return      Body custom gravity / orxNULL is object doesn't have any
 */
orxVECTOR *orxFASTCALL orxBody_GetCustomGravity(const orxBODY *_pstBody, orxVECTOR *_pvCustomGravity)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvCustomGravity != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Updates result */
    pvResult = orxPhysics_GetCustomGravity(_pstBody->pstData, _pvCustomGravity);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Gets a body center of mass
 * @param[in]   _pstBody        Concerned body
 * @param[out]  _pvMassCenter   Mass center to get
 * @return      Mass center / orxNULL
 */
orxVECTOR *orxFASTCALL orxBody_GetMassCenter(const orxBODY *_pstBody, orxVECTOR *_pvMassCenter)
{
  orxVECTOR *pvResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvMassCenter != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Gets mass center */
    pvResult = orxPhysics_GetMassCenter(_pstBody->pstData, _pvMassCenter);
  }
  else
  {
    /* Updates result */
    pvResult = orxNULL;
  }

  /* Done! */
  return pvResult;
}

/** Applies a torque
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _fTorque        Torque to apply
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyTorque(orxBODY *_pstBody, orxFLOAT _fTorque)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Applies torque */
    eResult = orxPhysics_ApplyTorque(_pstBody->pstData, _fTorque);
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Applies a force
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvForce        Force to apply
 * @param[in]   _pvPoint        Point (world coordinates) where the force will be applied, if orxNULL, center of mass will be used
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyForce(orxBODY *_pstBody, const orxVECTOR *_pvForce, const orxVECTOR *_pvPoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvForce != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Has given point? */
    if(_pvPoint != orxNULL)
    {
      /* Applies force */
      eResult = orxPhysics_ApplyForce(_pstBody->pstData, _pvForce, _pvPoint);
    }
    else
    {
      orxVECTOR vMassCenter;

      /* Applies force on mass center */
      eResult = orxPhysics_ApplyForce(_pstBody->pstData, _pvForce, orxPhysics_GetMassCenter(_pstBody->pstData, &vMassCenter));
    }
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Applies an impulse
 * @param[in]   _pstBody        Concerned body
 * @param[in]   _pvImpulse      Impulse to apply
 * @param[in]   _pvPoint        Point (world coordinates) where the impulse will be applied, if orxNULL, center of mass will be used
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
orxSTATUS orxFASTCALL orxBody_ApplyImpulse(orxBODY *_pstBody, const orxVECTOR *_pvImpulse, const orxVECTOR *_pvPoint)
{
  orxSTATUS eResult;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);
  orxASSERT(_pvImpulse != orxNULL);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    /* Has given point? */
    if(_pvPoint != orxNULL)
    {
      /* Applies impulse */
      eResult = orxPhysics_ApplyImpulse(_pstBody->pstData, _pvImpulse, _pvPoint);
    }
    else
    {
      orxVECTOR vMassCenter;

      /* Applies impusle on mass center */
      eResult = orxPhysics_ApplyForce(_pstBody->pstData, _pvImpulse, orxPhysics_GetMassCenter(_pstBody->pstData, &vMassCenter));
    }
  }
  else
  {
    /* Updates result */
    eResult = orxSTATUS_FAILURE;
  }

  /* Done! */
  return eResult;
}

/** Issues a raycast to test for potential bodies in the way
 * @param[in]   _pvStart        Start of raycast
 * @param[in]   _pvEnd          End of raycast
 * @param[in]   _u16SelfFlags   Selfs flags used for filtering (0xFFFF for no filtering)
 * @param[in]   _u16CheckMask   Check mask used for filtering (0xFFFF for no filtering)
 * @param[in]   _bEarlyExit     Should stop as soon as an object has been hit (which might not be the closest)
 * @param[in]   _pvContact      If non-null and a contact is found it will be stored here
 * @param[in]   _pvNormal       If non-null and a contact is found, its normal will be stored here
 * @return Colliding orxBODY / orxNULL
 */
orxBODY *orxFASTCALL orxBody_Raycast(const orxVECTOR *_pvStart, const orxVECTOR *_pvEnd, orxU16 _u16SelfFlags, orxU16 _u16CheckMask, orxBOOL _bEarlyExit, orxVECTOR *_pvContact, orxVECTOR *_pvNormal)
{
  orxHANDLE hRaycastResult;
  orxBODY  *pstResult = orxNULL;

  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxASSERT(_pvStart != orxNULL);
  orxASSERT(_pvEnd != orxNULL);

  /* Issues raycast */
  hRaycastResult = orxPhysics_Raycast(_pvStart, _pvEnd, _u16SelfFlags, _u16CheckMask, _bEarlyExit, _pvContact, _pvNormal);

  /* Found? */
  if(hRaycastResult != orxHANDLE_UNDEFINED)
  {
    /* Updates result */
    pstResult = orxBODY(hRaycastResult);
  }

  /* Done! */
  return pstResult;
}

/** Applies physics simulation result to the Body
 * @param[in]   _pstBody                      Concerned body
 * @return      orxSTATUS_SUCCESS / orxSTATUS_FAILURE
 */
void orxFASTCALL orxBody_ApplySimulationResult(orxBODY *_pstBody)
{
  /* Checks */
  orxASSERT(sstBody.u32Flags & orxBODY_KU32_STATIC_FLAG_READY);
  orxSTRUCTURE_ASSERT(_pstBody);

  /* Has data? */
  if(orxStructure_TestFlags(_pstBody, orxBODY_KU32_FLAG_HAS_DATA))
  {
    orxFRAME_SPACE  eFrameSpace;
    orxFRAME       *pstFrame;
    orxOBJECT      *pstOwner;

    /* Gets ower */
    pstOwner = orxOBJECT(_pstBody->pstOwner);

    /* Gets its frame */
    pstFrame = orxOBJECT_GET_STRUCTURE(pstOwner, FRAME);

    /* Gets its frame space */
    eFrameSpace = (orxFrame_IsRootChild(pstFrame) != orxFALSE) ? orxFRAME_SPACE_LOCAL : orxFRAME_SPACE_GLOBAL;

    /* Owner enabled? */
    if(orxObject_IsEnabled(pstOwner) != orxFALSE)
    {
      orxVECTOR             vPosition, vSpeed, vDiff;
      orxFLOAT              fZBackup, fRotation, fDiff;
      orxFLOAT              fSpeedCoef;
      const orxCLOCK_INFO  *pstClockInfo;
      orxCLOCK             *pstClock;

      /* Gets owner clock */
      pstClock = orxObject_GetClock(pstOwner);

      /* Gets corresponding clock info */
      pstClockInfo = (pstClock != orxNULL) ? orxClock_GetInfo(pstClock) : orxNULL;

      /* Has a multiply modifier? */
      if((pstClockInfo != orxNULL) && (pstClockInfo->eModType == orxCLOCK_MOD_TYPE_MULTIPLY))
      {
        /* Gets speed coef */
        fSpeedCoef = (pstClockInfo->fModValue != _pstBody->fTimeMultiplier) ? pstClockInfo->fModValue / _pstBody->fTimeMultiplier : orxFLOAT_1;

        /* Stores multiplier */
        _pstBody->fTimeMultiplier = pstClockInfo->fModValue;
      }
      else
      {
        /* Reverts speed coef */
        fSpeedCoef = (_pstBody->fTimeMultiplier != orxFLOAT_1) ? orxFLOAT_1 / _pstBody->fTimeMultiplier : orxFLOAT_1;

        /* Stores multiplier */
        _pstBody->fTimeMultiplier = orxFLOAT_1;
      }

      /* Gets current position */
      orxFrame_GetPosition(pstFrame, eFrameSpace, &vPosition);

      /* Backups its Z */
      fZBackup = vPosition.fZ;

      /* Global space? */
      if(eFrameSpace == orxFRAME_SPACE_GLOBAL)
      {
        /* Computes diff vector & rotation */
        orxVector_Set(&vDiff, vPosition.fX - _pstBody->vPreviousPosition.fX, vPosition.fY - _pstBody->vPreviousPosition.fY, orxFLOAT_0);
        fDiff = orxFrame_GetRotation(pstFrame, eFrameSpace) - _pstBody->fPreviousRotation;
      }

      /* Gets body up-to-date position */
      orxPhysics_GetPosition(_pstBody->pstData, &vPosition);

      /* Restores Z */
      vPosition.fZ = fZBackup;

      /* Gets body up-to-date rotation */
      fRotation = orxPhysics_GetRotation(_pstBody->pstData);

      /* Global space? */
      if(eFrameSpace == orxFRAME_SPACE_GLOBAL)
      {
        /* Updates position & rotation with diffs */
        orxVector_Add(&vPosition, &vPosition, &vDiff);
        fRotation += fDiff;

        /* Stores them */
        orxPhysics_SetPosition(_pstBody->pstData, &vPosition);
        orxPhysics_SetRotation(_pstBody->pstData, fRotation);
        orxVector_Copy(&(_pstBody->vPreviousPosition), &vPosition);
        _pstBody->fPreviousRotation = fRotation;
      }

      /* Updates position */
      orxFrame_SetPosition(pstFrame, eFrameSpace, &vPosition);

      /* Updates rotation */
      orxFrame_SetRotation(pstFrame, eFrameSpace, fRotation);

      /* Updates its angular velocity */
      orxPhysics_SetAngularVelocity(_pstBody->pstData, orxPhysics_GetAngularVelocity(_pstBody->pstData) * fSpeedCoef);

      /* Updates its speed */
      orxPhysics_SetSpeed(_pstBody->pstData, orxVector_Mulf(&vSpeed, orxPhysics_GetSpeed(_pstBody->pstData, &vSpeed), fSpeedCoef));

      /* Has speed coef */
      if(fSpeedCoef != orxFLOAT_1)
      {
        orxVECTOR vGravity;

        /* No custom gravity */
        if(orxBody_GetCustomGravity(_pstBody, &vGravity) == orxNULL)
        {
          /* Uses world gravity */
          orxPhysics_GetGravity(&vGravity);
        }

        /* Applies modified gravity */
        orxBody_SetCustomGravity(_pstBody, orxVector_Mulf(&vGravity, &vGravity, fSpeedCoef));
      }
    }
    else
    {
      orxVECTOR vPosition;

      /* Enforces its body properties */
      orxPhysics_SetRotation(_pstBody->pstData, orxFrame_GetRotation(pstFrame, eFrameSpace));
      orxPhysics_SetAngularVelocity(_pstBody->pstData, orxFLOAT_0);
      orxPhysics_SetPosition(_pstBody->pstData, orxFrame_GetPosition(pstFrame, eFrameSpace, &vPosition));
      orxPhysics_SetSpeed(_pstBody->pstData, &orxVECTOR_0);
    }
  }

  /* Done! */
  return;
}
