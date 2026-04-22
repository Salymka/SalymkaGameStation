import argparse
import math
from pathlib import Path

import numpy as np
import librosa


def hz_to_buzzer_freq(hz: float) -> int:
    """Clamp frequency to a practical passive-buzzer range."""
    if not np.isfinite(hz) or hz <= 0:
        return 0
    return int(np.clip(round(hz), 100, 4000))


def merge_notes(notes, min_duration_ms=30):
    """Merge neighboring identical notes and drop tiny fragments."""
    if not notes:
        return []

    merged = [list(notes[0])]
    for freq, dur in notes[1:]:
        if freq == merged[-1][0]:
            merged[-1][1] += dur
        else:
            merged.append([freq, dur])

    # remove too-short fragments by merging into previous/next note
    cleaned = []
    for i, (freq, dur) in enumerate(merged):
        if dur >= min_duration_ms or not cleaned:
            cleaned.append([freq, dur])
        else:
            cleaned[-1][1] += dur

    return [(int(freq), int(dur)) for freq, dur in cleaned]


def audio_to_song(
    input_file: str,
    song_name: str = "custom_sound",
    target_sr: int = 22050,
    frame_length: int = 2048,
    hop_length: int = 512,
    fmin: float = 80.0,
    fmax: float = 3000.0,
    silence_db: float = 35.0,
    quantize_to_notes: bool = False,
):
    # Load mono audio
    y, sr = librosa.load(path=input_file, sr=target_sr, mono=True)

    # Trim leading/trailing silence
    y, _ = librosa.effects.trim(y, top_db=silence_db)

    # Pitch tracking
    f0, voiced_flag, voiced_prob = librosa.pyin(
        y,
        fmin=fmin,
        fmax=fmax,
        sr=sr,
        frame_length=frame_length,
        hop_length=hop_length,
    )

    frame_ms = hop_length / sr * 1000.0

    notes = []
    for hz, voiced in zip(f0, voiced_flag):
        if (hz is None) or (not voiced) or (not np.isfinite(hz)):
            freq = 0
        else:
            if quantize_to_notes:
                midi = librosa.hz_to_midi(hz)
                hz = librosa.midi_to_hz(round(midi))
            freq = hz_to_buzzer_freq(float(hz))

        dur = int(round(frame_ms))
        notes.append((freq, dur))

    notes = merge_notes(notes, min_duration_ms=max(20, int(frame_ms)))

    # C++ output
    array_name = f"{song_name.upper()}_NOTES"
    lines = []
    lines.append(f"constexpr Note {array_name}[] = {{")
    for freq, dur in notes:
        lines.append(f"    {{{freq}, {dur}}},")
    lines.append("};")
    lines.append("")
    lines.append(f"constexpr Song {song_name} = {{")
    lines.append(f"    {array_name},")
    lines.append(f"    sizeof({array_name}) / sizeof(Note)")
    lines.append("};")

    return "\n".join(lines), notes


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input", help="Path to mp3/wav/ogg file")
    parser.add_argument("--name", default="custom_sound", help="C++ song variable name")
    parser.add_argument("--notes", action="store_true", help="Quantize to musical notes")
    args = parser.parse_args()

    cpp, notes = audio_to_song(
        input_file=args.input,
        song_name=args.name,
        quantize_to_notes=args.notes,
    )

    out_path = Path(args.input).with_suffix(".h.txt")
    out_path.write_text(cpp, encoding="utf-8")

    print(cpp)
    print(f"\nSaved to: {out_path}")
    print(f"Generated {len(notes)} merged notes")


if __name__ == "__main__":
    main()