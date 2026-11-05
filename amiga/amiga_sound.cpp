// Pingus' sound on SDL2_mixer, for the Amiga port: see amiga_sound.hpp.
// The AmiCradle Team. GPL 3 or later, like Pingus.

#include "amiga_sound.hpp"

#include <SDL.h>
#include <SDL_mixer.h>
#include <algorithm>
#include <stdexcept>

#include <logmich/log.hpp>

#include "pingus/globals.hpp"
#include "pingus/path_manager.hpp"

namespace pingus::sound {

namespace {

int mix_volume(float v)
{
  return static_cast<int>(std::clamp(v, 0.0f, 1.0f) * MIX_MAX_VOLUME);
}

} // namespace

PingusSoundMixer::PingusSoundMixer() :
  m_chunks(),
  m_music(nullptr),
  m_music_gain(1.0f),
  m_music_volume(1.0f),
  m_sound_volume(1.0f),
  m_master_volume(1.0f)
{
  if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0)
    throw std::runtime_error(std::string("SDL audio: ") + SDL_GetError());

  // 22 kHz stereo is plenty for Pingus' 8-bit WAVs and its tracker songs,
  // and half the work of 44 kHz.
  if (Mix_OpenAudio(22050, AUDIO_S16SYS, 2, 1024) != 0)
  {
    std::string err = Mix_GetError();
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    throw std::runtime_error("Mix_OpenAudio: " + err);
  }
  if ((Mix_Init(MIX_INIT_MOD) & MIX_INIT_MOD) == 0)
    log_warn("SDL_mixer has no tracker music: {}", Mix_GetError());
  Mix_AllocateChannels(16);
}

PingusSoundMixer::~PingusSoundMixer()
{
  real_stop_music();
  Mix_HaltChannel(-1);
  for (auto& it : m_chunks)
    if (it.second)
      Mix_FreeChunk(it.second);
  m_chunks.clear();
  Mix_CloseAudio();
  Mix_Quit();
  SDL_QuitSubSystem(SDL_INIT_AUDIO);
}

void
PingusSoundMixer::update(float /*delta*/)
{
}

void
PingusSoundMixer::real_play_sound(std::string const& name, float volume, float panning)
{
  if (!globals::sound_enabled || m_master_volume <= 0 || m_sound_volume <= 0)
    return;

  Mix_Chunk* chunk = nullptr;
  auto it = m_chunks.find(name);
  if (it != m_chunks.end())
  {
    chunk = it->second;
  }
  else
  {
    std::string filename = g_path_manager.complete("sounds/" + name + ".wav");
    chunk = Mix_LoadWAV(filename.c_str());
    if (!chunk)
      log_error("can't load sound {}: {}", filename, Mix_GetError());
    m_chunks[name] = chunk; // a missing one is not tried again
  }
  if (!chunk)
    return;

  int channel = Mix_PlayChannel(-1, chunk, 0);
  if (channel < 0)
    return; // all channels busy
  Mix_Volume(channel, mix_volume(volume * m_sound_volume * m_master_volume));
  float p = std::clamp(panning, -1.0f, 1.0f);
  Uint8 left  = static_cast<Uint8>(255 * std::min(1.0f, 1.0f - p));
  Uint8 right = static_cast<Uint8>(255 * std::min(1.0f, 1.0f + p));
  Mix_SetPanning(channel, left, right);
}

void
PingusSoundMixer::real_stop_music()
{
  if (m_music)
  {
    Mix_HaltMusic();
    Mix_FreeMusic(m_music);
    m_music = nullptr;
  }
}

void
PingusSoundMixer::real_play_music(std::string const& filename, float volume, bool loop)
{
  if (!globals::music_enabled || m_master_volume <= 0 || m_music_volume <= 0)
    return;

  real_stop_music();
  m_music = Mix_LoadMUS(filename.c_str());
  if (!m_music)
  {
    log_error("can't load music {}: {}", filename, Mix_GetError());
    return;
  }
  m_music_gain = volume;
  apply_music_volume();
  if (Mix_PlayMusic(m_music, loop ? -1 : 1) != 0)
    log_error("can't play music {}: {}", filename, Mix_GetError());
}

void
PingusSoundMixer::apply_music_volume()
{
  Mix_VolumeMusic(mix_volume(m_music_gain * m_music_volume * m_master_volume));
}

void
PingusSoundMixer::set_sound_volume(float volume)
{
  m_sound_volume = volume;
}

void
PingusSoundMixer::set_music_volume(float volume)
{
  m_music_volume = volume;
  apply_music_volume();
}

void
PingusSoundMixer::set_master_volume(float volume)
{
  m_master_volume = volume;
  apply_music_volume();
}

float PingusSoundMixer::get_sound_volume() const { return m_sound_volume; }
float PingusSoundMixer::get_music_volume() const { return m_music_volume; }
float PingusSoundMixer::get_master_volume() const { return m_master_volume; }

} // namespace pingus::sound
