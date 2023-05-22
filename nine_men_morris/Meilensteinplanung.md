## <center>Mühle (Nine men's morris) Spiel<center>
## <center>SysPrak-Gruppe 25<center>
### **Projektaufgabe (Teil 1 von 4)**
> #### Zeitleiste: 28.11 - 4.12
> - Unterteilung: 
> >  - Yujie Lin:
> >     ```
> >     Für Datei connectServer.h&c zuständig, d.h. für den Aufbau der Verbindung zum Server. 
> >     ```
> >  - Xiyue Zheng:
> >     ```
> >     Für performConnection.c&h zuständig. Es gibt zwei Methoden in dieser Klasse, eine um die Prolog-Phase zu starten, und die andere um die vom Server empfangenen Informationen nach Bedarf zu formatieren
> >     ```
> >  - Bowen Zhou
> >     ```
> >     Für prolog-phase.h&c zuständig. Die Methode prologphase in dieser Klasse muss in performConnection aufgerufen werden
> >     ```
> >  - Zonggen Li
> >     ```
> >     Für die Erstellung der Klasse sysprak-client zuständig, die main-Methode ist in dieser Klasse, die Kommandozeilenargumente (-g und -p) erhält. 
> >     Und für makefile zuständig
> >     ```
### **Projektaufgabe (Teil 2 von 4)**
> #### Zeitleiste: 5.12 - 18.12
> - Unterteilung: 
>>   - Zonggen
>>     ```
>>     Verantwortlich für die Erstellung der Datei client.conf und die Erstellung von Strukturen zur Aufnahme der aus conf und conf-info.h gelesenen Informationen.
>>     Und verantwortlich für die Erstellung von config.c zum Lesen der Konfigurationsdatei.
>>     ```
>>   - Xiyue
>>     ```
>>     Verantwortlich für die Erstellung von Eltern- und Kindprozessen in sysprak-client, mit connector als Kind und thinker als Elternteil
>>     ```
>>   - 5 - Yujie
>>     ```
>>     Verantwortlich für die Erstellung der Shared-Memory-Strukturen shared-memory.h und game-info.h Einbindung von game-info.h in Shared Memory über #include
>>     ```
>>   - 6 - bowen
>>     ```
>>     Verantwortlich für die Erstellung der Datei shared-memory.c, die in der Hauptmethode aufgerufen wird und gemeinsamen Speicher erzeugt
>>     ```

### **Projektaufgabe (Teil 3 von 4)**
> #### Zeitleiste: 19.12.2022 - 8.1.2023
> - Unterteilung: 
> >  - Yujie
> >    ```
> >    Verantwortlich für die Erstellung der Datei gamephase.c&h (die dem Spielverlauf entspricht), für die Ermittlung des Protokollbefehls, den der Server sendet, und dessen Überprüfung sowie für das Senden von Anweisungen wie z. B. thinking.
> >    Dann wird der Denker benachrichtigt, nachdem der Denkbefehl gesendet wurde (über die Kill-Methode)
> >    ```
> >  - Xiyue
> >    ```
> >    Verantwortlich für die Erstellung der gameover-phase.c&h
> >    Und verantwortlich für die Erstellung der Pipe in sysprak-client (vor der Erstellung des übergeordneten Prozesses) und die Verwaltung der Nachrichten, die über epoll() oder select auf dem Connector-Prozess empfangen werden
> >    ```
> >  - Bowen
> >    ```
> >    Verantwortlich für die Implementierung der think-Methode in thinker.c, um festzustellen, ob die Spielzug-Phase mit dem thinkflag gestartet werden kann, und wenn ja, den Aufruf der Methode in gamelogic.
> >    Und verantwortlich für die Implementierung der print-game-situation-Methode in thinker.c
> >    ```
> >  - Zonggen
> >    ```
> >    Verantwortlich für die Spiellogikdatei gamelogic.c
> >    ```

### **Projektaufgabe (Teil 4 von 4)**
> #### Zeitleiste: 9.1.2023 - 29.1.2023
> > Sicherzustellen, dass das Projekt vollständig und durchführbar ist und dass die Zeit- und Raumeffizienz des Projekts innerhalb der erforderlichen Grenzen liegt.