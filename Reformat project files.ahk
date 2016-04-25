Run, C:\Program Files\Alex Leykin\New Vision\NewVision.exe
SetTitleMatchMode, 2
SetTitleMatchMode, slow
SetKeyDelay, -1
WinWait, Untitled - NewVision,, 2
WinWaitActive, Untitled - NewVision,, 2
first := 1
Loop, F:\My Documents\My Research\Code\C++\Projects\NewVision\Projects\*.proj
{
  Send, {CTRLDOWN}o{CTRLUP}
  WinWait, ahk_class #32770,
  IfWinNotActive, Open, , WinActivate, Open
  WinWaitActive, Open, 
  SendRaw, %A_LoopFileFullPath%
  Send, {ENTER}
  if first = 1
  {
    first := 0
    WinWaitActive, NewVision
    Send, {TAB}{SPACE}
  }
  WinWaitActive, NewVision
  Send, {CTRLDOWN}s{CTRLUP}
  WinWaitActive, NewVision
}
Send, {ALTDOWN}{F4}{ALTUP}
Exit