# Robo Cam - "robo_cam.ino"

## Funktionen

Um den Roboter zu bewegen, muss man zuerst die Position in der Array definieren. Dafür ruft man die Funktion `set_pos(int index, int value);` auf. Diese funktion stellt sicher, dass der Wert nicht auserhalb des 
erlaubten Wertebereichs ist.

Die indices entsprechen folgenden Gelenken/Servos:

 - 0 -> base
 - 1 -> shoulder
 - 2 -> elbow
 - 3 -> wrist_rot
 - 4 -> wrist_ver
 - 5 -> gripper

Nachdem die Position aller zu bewegenden Gelenke gesetzt ist, muss man `commit_pos();` ausführen, welche dann den Roboter dementsprechend bewegt.

---

Um den Roboter einfacher zu bewegen gibt es die Funktionen `move_abs(*int);` und `move_rel(*int)`. Die integer liste iste ein Array mit so vielen Elementen, wie es Gelenke angeschlossen sind (startet mit index 0 und zählt hoch).
Dies wird am Anfang des Programms in `MOTOR_COUNT` festgelegt.

`move_abs(*int)` bewegt den Roboter auf eine absolute Position, `move_rel(*int)` auf eine relative Position (hier sind negative Werte valid).


## Response Codes

Wenn man manche Funktionen ausführt geben diese manchmal eine dreistellige integer (base 10) zurück. Dies sind die response codes. 
Was diese beuten findet man unten:

 - 666 -> STARTED -> Das Programm ist bereit für Befehle
 - 202 -> SUCCES -> Der Befehl war erfolgreich 
 - 403 -> OUT OF BOUNDS -> Die angegebene Position ist auserhalb des erlaubten Wertebereichs
