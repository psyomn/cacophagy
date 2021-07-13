# psycal

Very simple, calendar for text, or other window managers.

    psycal -d

runs the daemon

    psycal -t 4:30 -m meeting with the people

will book a meeting at 4h30 the same day

    psycal -t 2021-06-01T4:30 meeting

will book a meeting at 4h30 the given day.

## libnotify

If your system has libnotify, you can run bazel with the feature, to
build with notifications for your desktop environment.

## design considerations

- should vacuum the data which is before `now()` into another read only
  file.

- keep file format simple of `timestamp`-`message`.

- add libnotify support if available on the system this is being
  compiled on
