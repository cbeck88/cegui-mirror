/***********************************************************************
    filename:   CEGUIAnimationInstance.h
    created:    7/8/2010
    author:     Martin Preisler

    purpose:    Defines the interface for the AnimationInstance class
*************************************************************************/
/***************************************************************************
 *   Copyright (C) 2004 - 2010 Paul D Turner & The CEGUI Development Team
 *
 *   Permission is hereby granted, free of charge, to any person obtaining
 *   a copy of this software and associated documentation files (the
 *   "Software"), to deal in the Software without restriction, including
 *   without limitation the rights to use, copy, modify, merge, publish,
 *   distribute, sublicense, and/or sell copies of the Software, and to
 *   permit persons to whom the Software is furnished to do so, subject to
 *   the following conditions:
 *
 *   The above copyright notice and this permission notice shall be
 *   included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *   IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 *   OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *   OTHER DEALINGS IN THE SOFTWARE.
 ***************************************************************************/
#ifndef _CEGUIAnimationInstance_h_
#define _CEGUIAnimationInstance_h_

#include "CEGUIEventArgs.h"
#include "CEGUIEvent.h"
#include <map>
#include <vector>

#if defined(_MSC_VER)
#   pragma warning(push)
#   pragma warning(disable : 4251)
#endif

// Start of CEGUI namespace section
namespace CEGUI
{

/*!
\brief
    EventArgs based class that holds information about which animation instnace
    fired given event.
*/
class CEGUIEXPORT AnimationEventArgs : public EventArgs
{
public:
    AnimationEventArgs(AnimationInstance* inst) : instance(inst) {}
    //! pointer to a AnimationInstance object of relevance to the event.
    AnimationInstance* instance;
};

/*!
\brief
    Defines an 'animation instance' class

    Animation classes hold definition of the animation. Whilst this class holds
    data needed to use the animation definition - target PropertySet, event
    receiver, animation position, ...

    You have to define animation first and then instantiate it via
    AnimationManager::instantiateAnimation

\see
    Animation
*/
class CEGUIEXPORT AnimationInstance
{
public:
    //! Namespace for animation instance events
    //! these are fired on event receiver, not this animation instance!
    static const String EventNamespace;

    //! fired when animation instance starts
    static const String EventAnimationStarted;
    //! fired when animation instance stops
    static const String EventAnimationStopped;
    //! fired when animation instance pauses
    static const String EventAnimationPaused;
    //! fired when animation instance unpauses
    static const String EventAnimationUnpaused;
    //! fired when animation instance ends
    static const String EventAnimationEnded;
    //! fired when animation instance loops
    static const String EventAnimationLooped;

    //! internal constructor, please use AnimationManager::instantiateAnimation
    AnimationInstance(Animation* definition);

    /** internal destructor, please use
     * AnimationManager::destroyAnimationInstance
     */
    ~AnimationInstance(void);

    /*!
    \brief
        Retrieves the animation definition that is used in this instance
    */
    Animation* getDefinition() const;

    /*!
    \brief
        Sets the target property set - this class will get it's properties
        affected by the Affectors!
    */
    void setTarget(PropertySet* target);

    /*!
    \brief
        Retrieves the target property set
    */
    PropertySet* getTarget() const;

    /*!
    \brief
        Sets event receiver - this class will receive events when something
        happens to the playback of this animation - it starts, stops, pauses,
        unpauses, ends and loops
    */
    void setEventReceiver(EventSet* receiver);

    /*!
    \brief
        Retrieves the event receiver
    */
    EventSet* getEventReceiver() const;

    /*!
    \brief
        Sets event sender - this class will send events and can affect this
        animation instance if there are any auto subscriptions defined in the
        animation definition
    */
    void setEventSender(EventSet* sender);

    /*!
    \brief
        Retrieves the event sender
    */
    EventSet* getEventSender() const;

    /*!
    \brief
        Helper method, sets given window as target property set, event receiver
        and event set
    */
    void setTargetWindow(Window* target);

    /*!
    \brief
        Sets playback position. Has to be higher or equal to 0.0 and lower or
        equal to Animation definition's duration.
    */
    void setPosition(float position);

    /*!
    \brief
        Retrieves current playback position
    */
    float getPosition() const;

    /*!
    \brief
        Sets playback speed - you can speed up / slow down individual instances
        of the same animation. 1.0 means normal playback.
    */
    void setSpeed(float speed);

    /*!
    \brief
        Retrieves current playback speed
    */
    float getSpeed() const;

    /*!
    \brief
        Starts this animation instance - sets position to 0.0 and unpauses

    \par
        This also causes base values to be purged!
    */
    void start();

    /*!
    \brief
        Stops this animation instance - sets position to 0.0 and pauses
    */
    void stop();

    /*!
    \brief
        Pauses this animation instance - stops it from stepping forward
    */
    void pause();

    /*!
    \brief
       Unpauses this animation instance - allows it to step forward again
    */
    void unpause();

    /*!
    \brief
       Pauses the animation if it's running and unpauses it if it isn't
    */
    void togglePause();

    /*!
    \brief
        Returns true if this animation instance is currently unpaused,
        if it is stepping forward.
    */
    bool isRunning() const;

    /*!
    \brief
        Internal method, steps the animation forward by the given delta
    */
    void step(float delta);

    /*!
    \brief
        handler that starts the animation instance
    */
    bool handleStart(const CEGUI::EventArgs& e);

    /*!
    \brief
        handler that stops the animation instance
    */
    bool handleStop(const CEGUI::EventArgs& e);

    /*!
    \brief
        handler that pauses the animation instance
    */
    bool handlePause(const CEGUI::EventArgs& e);

    /*!
    \brief
        handler that unpauses the animation instance
    */
    bool handleUnpause(const CEGUI::EventArgs& e);

    /*!
    \brief
        handler that toggles pause on this animation instance
    */
    bool handleTogglePause(const CEGUI::EventArgs& e);

private:
    // allow Affector to purge and get base values if needed
    friend class Affector;
    // allow animation to register auto connections
    friend class Animation;

    //! applies this animation instance
    void apply();

    //! this is called when animation starts
    void onAnimationStarted();
    //! this is called when animation stops
    void onAnimationStopped();
    //! this is called when animation pauses
    void onAnimationPaused();
    //! this is called when animation unpauses
    void onAnimationUnpaused();

    //! this is called when animation ends
    void onAnimationEnded();
    //! this is called when animation loops (in RM_Loop or RM_Bounce mode)
    void onAnimationLooped();

    /** this purges all base values forcing this class to gather new ones fresh
     * from the properties
     */
    void purgeBaseValues(void);
    /** retrieves base value, if it isn't cached already, it retrieves it fresh
     * from the properties
     */
    const String& getBaseValue(Affector* affector);

    //! add a reference to subscribed connection
    void addAutoConnection(Event::Connection conn);
    //! removes all auto subscribed connections
    void unsubscribeAutoConnections();

    //! parent Animation definition
    Animation* d_definition;

    //! target property set, properties of this are affected by Affectors
    PropertySet* d_target;
    //! event receiver, receives events about this animation instance
    EventSet* d_eventReceiver;
    /** event sender, sends events and can control this animation instance if
     * there are any auto subscriptions
     */
    EventSet* d_eventSender;

    /** position of this animation instance,
     * should always be higher or equal to 0.0 and lower or equal to duration of
     * animation definition
     */
    float d_position;
    //! playback speed, 1.0 means normal playback
    float d_speed;
    //! needed for RM_Bounce mode, if true, we bounce backwards
    bool d_bounceBackwards;
    //! true if this animation is unpaused
    bool d_running;

    typedef std::map<Affector*, String> BaseValueMap;
    /** cached base values, used for relative application method, see Affector
     * class
     */
    BaseValueMap d_baseValues;

    typedef std::vector<Event::Connection> ConnectionTracker;
    //! tracks auto event connections we make.
    ConnectionTracker d_autoConnections;
};

} // End of  CEGUI namespace section

#if defined(_MSC_VER)
#   pragma warning(pop)
#endif

#endif  // end of guard _CEGUIAnimationInstance_h_

