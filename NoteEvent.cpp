#include "Arduino.h"

#include "NoteEvent.h"

/**
 * NoteEvent::Event - initialize a new NoteEvent with a Note object
 * @t        - where on the timeline this NoteEvent occurs in ticks
 * @note     - The note (in MIDI note numbers) of the NoteEvent
 * @length   - The length of the note
 * @velocity - velocity of the note
 */
NoteEvent::NoteEvent( int t, int note, int length, int velocity) {
    ticks = t;
    Note *n = (Note*)malloc(sizeof(Note));

    n->note = note;
    n->length = length;
    n->velocity = velocity;
    n->list = (Note*)0;

    this->notes = n;
    next = (NoteEvent*)0;
}

/**
 * NoteEvent::~Event - Destroy the NoteEvent. Lists should not be assumed to be 
 * destroyed here since their members may be referenced elsewhere.
 * However, this will link prev and next pointers to each other.
 */
NoteEvent::~NoteEvent() {
    if (prev != 0)
        prev->next = next;
    if (next != 0)
        next->prev = prev;
}

/** 
 * NoteEvent::addNote - Insert a new note to the list of Notes. Returns the
 *                      start of the note list after insertion.
 * @t        - the timing of the note in ticks
 * @note     - the note number (MIDI number)
 * @length   - the length of the note
 * @velocity - velocity of the note
 */
NoteEvent* NoteEvent::add( int t, int note, int length, int velocity) {
    // If we're inserting before the first note in our list
    if (t < ticks) {
        NoteEvent *n = new NoteEvent(t, note, length, velocity);

        n->next = this;
        n->prev = (NoteEvent*)0;

        prev = n;

        return n;
    }
    // If timing is the same, add to this list
    else if (t == ticks) {
        // check if note already exists in this list (keyed by note number)
        for (Note *member = notes; member != NULL; member = member->list) {
            // Update the note data
            if (member->note == note) {
                member->length = length;
                member->velocity = velocity;
                return this;
            }
        }

        // Make the note
        Note* n = (Note*)malloc(sizeof(Note));
        n->note     = note;
        n->length   = length;
        n->velocity = velocity;

        // Place at top of stack
        n->list = notes;
        notes   = n;
    }
    // Place NoteEvent between this and the next if it belongs
    else if (t > ticks && t < next->getTime()) {
        
        NoteEvent *e = new NoteEvent(t, note, length, velocity);

        e->next = next;
        e->prev = this;

        next->prev = e;
        next = e;
    }
    // Send down the line if it belongs further
    else if (t > ticks && next != NULL) {
        next->add(t, note, length, velocity);
    }
    // Or if we're at the end of the list, insert it
    else if (t > ticks) {
        NoteEvent *e = new NoteEvent(t, note, length, velocity);

        e->prev = this;
        next = e;
    }

    return this;
}

/**
 * NoteEvent::move - move the NoteEvent on the timeline. Takes care of merging
 * shifting in the linked list
 * @ticks - absolute position to move to
 */
void NoteEvent::move(int ticks) {
    // move backward
    if (ticks < this->ticks) {
        if (ticks < next->ticks) {
            this->ticks = ticks;
        } else if (ticks == next->ticks) {
            next->merge(this);
        } else {
            // Iterate backward
        }
    } else if (ticks > this->ticks) {
        if (ticks > prev->ticks) {
            this->ticks = ticks;
        }
    }
}

/**
 * NoteEvent::getNext - gets the next NoteEvent
 */
NoteEvent* NoteEvent::getNext() {
    return next;
}

/**
 * NoteEvent::getNotes - gets the note list
 */
Note* NoteEvent::getNotes() {
    return this->notes;
}

/**
 * NoteEvent::merge - merge another NoteEvent into this one
 * @e - the NoteEvent to be merged
 */
void NoteEvent::merge( NoteEvent* e) {

}

/**
 * NoteEvent::removeNote - remove a note from the list of notes in this NoteEvent
 * @n - the note to be removed
 */
Note* NoteEvent::remove( int n) {
    // Iterate through notes to see if we have a match
    for (Note *note = notes; note != 0; note = note->list) {
        // Our first case is if the first note is the one we're looking for
        if (note->note == n) {
            Note *retNote = note;
            notes = retNote->list;
            return retNote;
        }
        // Our second is everything else
        else if (note->list != 0) {
            if (note->list->note == n) {
                Note *retNote = note->list;
                note->list = retNote->list;
                return retNote;
            }
        }
    }

    return (Note*)0;
}

/**
 * NoteEvent::getTime - gets the time of this NoteEvent in ticks
 */
int NoteEvent::getTime() {
    return ticks;
}