/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Generic video decoder interface.
 */

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <vector>

#include <boost/shared_ptr.hpp>

#include "src/common/types.h"
#include "src/common/rational.h"
#include "src/common/scopedptr.h"
#include "src/common/timestamp.h"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"

#include "src/sound/types.h"

namespace Graphics {
	class Surface;
}

namespace Sound {
	class AudioStream;
	class QueuingAudioStream;
}

namespace Video {

/** A generic interface for video decoders. */
class VideoDecoder : public Graphics::GLContainer, public Graphics::Renderable {
public:
	enum Scale {
		kScaleNone,  ///< Don't scale the video.
		kScaleUp,    ///< Only scale the video up, if necessary.
		kScaleDown,  ///< Only scale the video down, if necessary.
		kScaleUpDown ///< Scale the video up and down, if necessary.
	};

	VideoDecoder();
	~VideoDecoder();

	void setScale(Scale scale);

	/** Is the video currently playing? */
	bool isPlaying() const;

	/**
	 * Returns the width of the video's frames.
	 *
	 * By default, this finds the largest width between all of the loaded
	 * tracks. However, a subclass may override this if it does any kind
	 * of post-processing on it.
	 *
	 * @return the width of the video's frames
	 */
	virtual uint32 getWidth() const;

	/**
	 * Returns the height of the video's frames.
	 *
	 * By default, this finds the largest height between all of the loaded
	 * tracks. However, a subclass may override this if it does any kind
	 * of post-processing on it.
	 *
	 * @return the height of the video's frames
	 */
	virtual uint32 getHeight() const;

	/** Start playing the video. */
	void start();

	/** Abort the playing of the video. */
	void abort();

	/**
	 * Check whether a new frame should be decoded, i.e. because enough
	 * time has elapsed since the last frame was decoded.
	 * @return whether a new frame should be decoded or not
	 */
	bool needsUpdate() const;

	/**
	 * Returns if the video has reached the end or not.
	 * @return true if the video has finished playing, false otherwise
	 */
	bool endOfVideo() const;

	/**
	 * Have all video tracks finished?
	 */
	bool endOfVideoTracks() const;

	/** Returns the time position (in ms) of the current video. */
	uint32 getTime() const;

	/** Return the time, in milliseconds, to the next frame. */
	uint32 getTimeToNextFrame() const;

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

	/**
	 * Get the duration of the video.
	 *
	 * If the duration is unknown, this will return 0. If this is not
	 * overriden, it will take the length of the longest track.
	 */
	virtual Common::Timestamp getDuration() const;

	/**
	 * Pause or resume the video. This should stop/resume any audio playback
	 * and other stuff. The initial pause time is kept so that any timing
	 * variables can be updated appropriately.
	 *
	 * This is a convenience method which automatically keeps track on how
	 * often the video has been paused, ensuring that after pausing an video
	 * e.g. twice, it has to be unpaused twice before actuallying resuming.
	 *
	 * @param pause		true to pause the video, false to resume it
	 */
	void pauseVideo(bool pause);

	/**
	 * Return whether the video is currently paused or not.
	 */
	bool isPaused() const { return _pauseLevel != 0; }

protected:
	/**
	 * An abstract representation of a track in a movie. Since tracks here are designed
	 * to work independently, they should not reference any other track(s) in the video.
	 */
	class Track {
	public:
		Track();
		virtual ~Track() {}

		/**
		 * The types of tracks this class can be.
		 */
		enum TrackType {
			/**
			 * The track type is unknown.
			 */
			kTrackTypeNone,

			/**
			 * The track is an audio track.
			 */
			kTrackTypeAudio,

			/**
			 * The track is a video track.
			 */
			kTrackTypeVideo
		};

		/**
		 * Get the type of track.
		 *
		 * This must not return kTrackTypeNone.
		 */
		virtual TrackType getTrackType() const = 0;

		/**
		 * Return if the track has finished.
		 */
		virtual bool endOfTrack() const = 0;

		/**
		 * Set the pause status of the track.
		 */
		void pause(bool shouldPause);

		/**
		 * Return if the track is paused.
		 */
		bool isPaused() const { return _paused; }

		/**
		 * Get the duration of the track.
		 *
		 * By default, this returns 0 for unknown.
		 */
		virtual Common::Timestamp getDuration() const;

	protected:
		/**
		 * Function called by pause() for subclasses to implement.
		 */
		virtual void pauseIntern(bool shouldPause);

	private:
		bool _paused; ///< Is the track paused?
	};

	/**
	 * An abstract representation of a video track.
	 */
	class VideoTrack : public Track {
	public:
		VideoTrack() {}
		virtual ~VideoTrack() {}

		TrackType getTrackType() const  { return kTrackTypeVideo; }
		virtual bool endOfTrack() const;

		/**
		 * Get the width of this track.
		 */
		virtual uint32 getWidth() const = 0;

		/**
		 * Get the height of this track.
		 */
		virtual uint32 getHeight() const = 0;

		/**
		 * Get the current frame of this track
		 */
		virtual int getCurFrame() const = 0;

		/**
		 * Get the frame count of this track
		 *
		 * @note If the frame count is unknown, return 0 (which is also
		 * the default implementation of the function). However, one must
		 * also implement endOfTrack() in that case.
		 */
		virtual int getFrameCount() const { return 0; }

		/**
		 * Get the start time of the next frame since the start of the video.
		 */
		virtual Common::Timestamp getNextFrameStartTime() const = 0;

		/**
		 * Get the time the given frame should be shown.
		 *
		 * By default, this returns a negative (invalid) value. This function
		 * should only be used by VideoDecoder::seekToFrame().
		 */
		virtual Common::Timestamp getFrameTime(uint frame) const;
	};

	/**
	 * A VideoTrack that is played at a constant rate.
	 *
	 * If the frame count is unknown, you must override endOfTrack().
	 */
	class FixedRateVideoTrack : public VideoTrack {
	public:
		FixedRateVideoTrack() {}
		virtual ~FixedRateVideoTrack() {}

		Common::Timestamp getNextFrameStartTime() const;
		virtual Common::Timestamp getDuration() const;
		Common::Timestamp getFrameTime(uint frame) const;

		/**
		 * Get the frame that should be displaying at the given time. This is
		 * helpful for someone implementing seek().
		 */
		uint getFrameAtTime(const Common::Timestamp &time) const;

	protected:
		/**
		 * Get the rate at which this track is played.
		 */
		virtual Common::Rational getFrameRate() const = 0;
	};

	/**
	 * An abstract representation of an audio track.
	 */
	class AudioTrack : public Track {
	public:
		AudioTrack();
		virtual ~AudioTrack() {}

		TrackType getTrackType() const { return kTrackTypeAudio; }

		bool endOfTrack() const;

		/**
		 * Start playing this track
		 */
		void start();

		/**
		 * Stop playing this track
		 */
		void stop();

		/**
		 * Get the gain for this track
		 */
		float getGain() const { return _gain; }

		/**
		 * Set the gain for this track
		 */
		void setGain(float gain);

		/**
		 * Get the time the AudioStream behind this track has been
		 * running
		 */
		uint32 getRunningTime() const;

		/**
		 * Mute the track
		 */
		void setMute(bool mute);

		/**
		 * Can more audio data be buffered?
		 */
		virtual bool canBufferData() const = 0;

	protected:
		void pauseIntern(bool shouldPause);

		/**
		 * Get the AudioStream that is the representation of this AudioTrack
		 */
		virtual Sound::AudioStream *getAudioStream() const = 0;

	private:
		Sound::ChannelHandle _handle;
		float _gain;
		bool _muted;
	};

	/**
	 * A Track pointer
	 */
	typedef boost::shared_ptr<Track> TrackPtr;

	/**
	 * A const Track pointer
	 */
	typedef boost::shared_ptr<const Track> ConstTrackPtr;

	/**
	 * An AudioTrack pointer
	 */
	typedef boost::shared_ptr<AudioTrack> AudioTrackPtr;

	/**
	 * A VideoTrack pointer
	 */
	typedef boost::shared_ptr<VideoTrack> VideoTrackPtr;

	/**
	 * A list of tracks
	 */
	typedef std::vector<TrackPtr> TrackList;

	/**
	 * A list of const tracks
	 */
	typedef std::vector<ConstTrackPtr> ConstTrackList;

	bool _needCopy; ///< Is new frame content available that needs to by copied?

	Common::ScopedPtr<Graphics::Surface> _surface; ///< The video's surface.

	/**
	 * Create a surface for video of these dimensions.
	 *
	 * Since the data will be copied into the graphics card memory, the surface's
	 * actual dimensions will be rounded up to the next power of two values.
	 *
	 * The surface's width and height is taken from getWidth() and getHeight().
	 *
	 * The surface's pixel format is always BGRA8888.
	 */
	void initVideo();

	void deinit();

	// GLContainer
	void doRebuild();
	void doDestroy();

	/**
	 * Decode enough data for the next frame
	 *
	 * Currently, audio may be buffered here at the same time. This is deprecated
	 * in favor of using checkAudioBuffer().
	 */
	virtual void decodeNextTrackFrame(VideoTrack &track) = 0;

	/**
	 * Ensure that there is enough audio buffered in the given track
	 * to reach the given timestamp.
	 */
	virtual void checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime);

	/**
	 * Define a track to be used by this class.
	 *
	 * The pointer is then owned by this base class.
	 *
	 * @param track The track to add
	 * @param isExternal Is this an external track not found by loadStream()?
	 */
	void addTrack(Track *track, bool isExternal = false);

	/**
	 * Get the given internal track based on its index.
	 *
	 * @return A valid track pointer on success, 0 otherwise
	 */
	TrackPtr getTrack(uint track);

	/**
	 * Get the given internal track based on its index
	 *
	 * @return A valid track pointer on success, 0 otherwise
	 */
	ConstTrackPtr getTrack(uint track) const;

	/**
	 * Set _nextVideoTrack to the video track with the lowest start time for the next frame.
	 *
	 * @return _nextVideoTrack
	 */
	VideoTrackPtr findNextVideoTrack();

	/**
	 * Get a copy of the internal tracks
	 */
	TrackList getInternalTracks() { return _internalTracks; }

	/**
	 * Get a copy of the internal tracks
	 */
	ConstTrackList getInternalTracks() const;

private:
	TrackList _tracks; ///< Tracks owned by this VideoDecoder (both internal and external).
	TrackList _internalTracks; ///< Tracks internal to this VideoDecoder.
	TrackList _externalTracks; ///< Tracks loaded from externals files.

	VideoTrackPtr _nextVideoTrack; ///< The next video track that needs to display.

	void stopAudio(); ///< Stop all audio tracks.
	void startAudio(); ///< Start the designated internal audio track and any external audio tracks.
	bool hasAudio() const;

	Graphics::TextureID _texture;

	float _textureWidth;
	float _textureHeight;

	Scale _scale;


	/** The start time of the video, or -1 for not set */
	uint32 _startTime;

	/** The pause level of the video; 0 for not paused. */
	uint32 _pauseLevel;

	/** The time when the track was first paused. */
	uint32 _pauseStartTime;

	/** Update the video, if necessary. */
	void update();

	/** Copy the video image data to the texture. */
	void copyData();

	/** Get the dimensions of the quad to draw the texture on. */
	void getQuadDimensions(float &width, float &height) const;
};

} // End of namespace Video

#endif // VIDEO_DECODER_H
