# AGENTS.md

## Project Overview

- Unreal Engine 5.8
- 3인칭 슈팅
- 싱글 플레이
- Windows Platform

프로젝트 목표, 범위 등 프로젝트에 대한 자세한 내용은 [PROJECT.md](Docs/PROJECT.md) 문서를 참고한다.

문서, 이슈, 코드에서 쓰는 용어는 [GLOSSARY.md](Docs/GLOSSARY.md) 문서를 따른다.

## Commands

프로젝트 루트에서 PowerShell로 실행한다. 아래 명령어를 실행하기 전에 다음 변수들을 정의한다.

```powershell
$UE = "C:\Program Files\Epic Games\UE_5.8"
$Project = "$PWD\Titan.uproject"
```

- 빌드
  - 에디터: `& "$UE\Engine\Build\BatchFiles\Build.bat" TitanEditor Win64 Development "-Project=$Project" -WaitMutex`
  - 게임: `& "$UE\Engine\Build\BatchFiles\Build.bat" Titan Win64 Development "-Project=$Project" -WaitMutex`
- 테스트: `& "$UE\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" $Project -ExecCmds="Automation RunTest <이름>; Quit" -unattended -nullrhi -nosplash -log`
- 패키징: `& "$UE\Engine\Build\BatchFiles\RunUAT.bat" BuildCookRun "-project=$Project" -platform=Win64 -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$PWD\Saved\Packages"`

## Architecture

### C++ and Blueprint

- Implement core gameplay rules, state, and state transitions in C++.
- Use Blueprint primarily for asset assignment, gameplay tuning, animation/VFX integration, and content-specific composition.
- Do not move gameplay logic to Blueprint solely for implementation convenience.
- Expose C++ APIs and properties to Blueprint only when Blueprint integration or content authoring actually requires them.
- Keep the authoritative gameplay state in C++; Blueprint may observe or configure it but should not maintain a competing source of truth.

### Data-Driven Gameplay

- Separate gameplay definitions and tunable values from gameplay behavior when those values represent content rather than implementation details.
- Do not hard-code content-specific values when they are expected to vary between weapons, enemies, pickups, or other content.
- Keep gameplay algorithms and complex control flow out of data definitions.
- Introduce a data abstraction only when there is an actual need for variation, reuse, authoring, or tuning.

When gameplay data is separated from code, consult [DataSystems](Docs/Architecture/References/DataSystems.md) to choose its container, references, and identifiers.

## Workflow

- Do not make implementation changes until the proposed approach has been reviewed and explicitly approved by the user.
- Before implementation, explain the intended changes, affected areas, and any significant design decisions.
- Discussion, investigation, and planning may proceed without approval; implementation requires explicit approval.
- If implementation requires a significant change from the approved approach, stop and obtain approval for the revised approach before proceeding.

### Source Control

- This project uses Git LFS locking for lockable Unreal assets.
- Before modifying a lockable asset, acquire its source-control lock and confirm that the lock succeeded.
- If another user holds the lock, do not modify the asset.
- Keep the lock until the completed change has been successfully pushed to the remote repository.
- If a locked asset was not changed or is no longer needed for the task, release the lock.
- Do not force-unlock another user's asset without explicit user approval.