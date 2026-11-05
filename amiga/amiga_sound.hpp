// Pingus' sound on SDL2_mixer, for the Amiga port (no OpenAL there):
// LIBS:OpenGPU/SDL2_mixer.module plays the WAV sounds and the tracker
// music (.it/.s3m). The AmiCradle Team. GPL 3 or later, like Pingus.

#ifndef HEADER_PINGUS_AMIGA_SOUND_HPP
#define HEADER_PINGUS_AMIGA_SOUND_HPP

#include <map>
#include <string>

#include "engine/sound/sound_impl.hpp"

struct Mix_Chunk;
struct Mix_Music;

namespace pingus::sound {

class PingusSoundMixer : public PingusSoundImpl
{
public:
  PingusSoundMixer();
  ~PingusSoundMixer() override;

  void update(float delta) override;

  void real_play_music(const std::string & filename, float volume, bool loop) override;
  void real_stop_music() override;
  void real_play_sound(const std::string & name, float volume, float panning) override;

  void set_sound_volume(float volume) override;
  void set_music_volume(float volume) override;
  void set_master_volume(float volume) override;

  float get_sound_volume() const override;
  float get_music_volume() const override;
  float get_master_volume() const override;

private:
  void apply_music_volume();

  std::map<std::string, Mix_Chunk*> m_chunks;
  Mix_Music* m_music;
  float m_music_gain;
  float m_music_volume;
  float m_sound_volume;
  float m_master_volume;

  PingusSoundMixer(PingusSoundMixer const&) = delete;
  PingusSoundMixer& operator=(PingusSoundMixer const&) = delete;
};

} // namespace pingus::sound

#endif
