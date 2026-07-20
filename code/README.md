# Lesson verification code

Every C++ snippet shown in a lesson must compile and pass its assertions here first.

```sh
c++ -std=c++20 -Wall -Wextra -o lesson1_check lesson1_check.cc && ./lesson1_check
c++ -std=c++20 -Wall -Wextra -o lesson2_ports lesson2_ports.cc && ./lesson2_ports
```

Convention (inherited from the DDD course): green assertions are evidence, not
decoration — each file proves the specific claim its lesson makes. Binaries are
git-ignored; sources are not.
