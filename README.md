# minishell-cso

Questo progetto consiste nell'implementazione di una shell bash-like, con le seguenti features:
- lettura input avanzata (readline) e gestione della history
- Gestione dei jobs (bg / fg / jobs)
- Pipe (|)
- redirections (>, >>, <, <<)
- gestione env (expander, export, env, unset)
- altre builtin base (cd, echo, exit, pwd)


## Main loop

la pipeline è la seguente:
- update jobs
- readline (and add line to history)
- update jobs
- lexer
- parser
- expander
- heredoc
- executor

## Readline
man: get a line from a user with editing

```bash
sudo apt-get install libreadline-dev
```

### Utilizzo

Permette di poter leggere in input una stringa, ammettendo editing (emacs style).
Gestisce la history dei comandi (add_history).
Permette l'uso del tab per il completamento, di default solo di file e dir, ma si può implementare una funzione custom per cercare i comandi builtin e presenti in PATH.
(https://thoughtbot.com/blog/tab-completion-in-gnu-readline e man)

Quando si usa il TAB per il completion, viene richiamata minishell_completion.

Se il tab viene usato all'inizio dell'input, viene tentato di completare cercando anche tra i comandi, altrimenti si cerca solo tra file e dir (default).

Nel caso in cui ci siano più possibilità di completamento, si dovrebbe ritornare la lista delle possibili soluzioni e il longest common prefix tra tutte le opzioni.
Per non dover implementare tutto ciò, si sfrutta rl_completion_matches, a cui va passata una funzione generator (command_generator) che verrà richiamata più volte. Ad ogni chiamata, questa dovrà restituire un possibile completamento e al termine ritornare NULL.

L'implementazione command_generator ha la seguente logica:

1. Alla prima iterazione, vengono inizializzate alcune variabili statiche che mantengono lo stato fino alla fine della ricerca

2. Vengono per prima cosa cercate corrispondenze tra i comandi builtin

3. Altrimenti si ricerca tra gli eseguibili all'interno delle cartelle presenti in PATH.
Viene aperta una dir, e poi si itera sulle entry all'interno. Quando terminano si prosegue con la dir successiva in PATH.
Per iterare correttamente le dir in PATH si utilizza strtok con il separatore ":".

4. Se non ci sono corrispondenze, interviene il comportamento di default di readline che completa con file e directory

Il comportamento finale è il seguente:
si scrive l'inizio del comando, poi si preme TAB per la prima volta.
Se c'è una sola corrispondenza, viene completata. Altrimenti, viene stampata la lista dei possibili completamenti e il comando viene completato con il longest common prefix.
Continuando a premere TAB, il comando viene completato iterando su ogni possibilità nella lista.
Per fare ciò vengono attivate le seguenti opzioni (man):
- show-all-if-ambiguous: words which have more than one possible completion cause the matches to be listed
- menu-complete-display-prefix: menu completion displays the common prefix of the list of possible completions
- rl_menu_complete : cycle through options on TAB

## Lexer

input: stringa del comando
output: lista di token

Ogni token ha:
- type (TokenType)
- text (per i token di tipo T_WORD)

Per gestire quotes e $ nell'expander, vengono rimpiazzate con dei marker (MARK_SQ, MARK_DQ, MARK_KEY)

quotes: '...' e "..."
il contenuto viene copiato letteralmente
Nelle double quote, viene gestito il backslash (\\, \$, \") e il $ (MARK_KEY)

backslash fuori dalle quotes: \char -> il char viene copiato letteralmente (\\ -> viene copiato \)

### Tipi di Token

- T_WORD

- T_PIPE = |

- T_RED_OUT = >
- T_RED_OUT_APP = >>
- T_RED_IN = <
- T_HEREDOC = <<

### Errori lessicali

- Quotes non chiuse
- Nessun carattere dopo \
- Input eccede la lunghezza massima

### Esempi
```bash
ls
[0] T_WORD (ls) | [1] T_NONE

echo ciao ciao
[0] T_WORD (echo) | [1] T_WORD (ciao) | [2] T_WORD (ciao) | [3] T_NONE
ciao ciao

echo "ciao ciao"
[0] T_WORD (echo) | [1] T_WORD (<DQ>ciao ciao<DQ>) | [2] T_NONE
ciao ciao

echo "$HOME"
[0] T_WORD (echo) | [1] T_WORD (<DQ><KEY>HOME<DQ>) | [2] T_NONE
/home/lorenzo

echo '$HOME'
[0] T_WORD (echo) | [1] T_WORD (<SQ>$HOME<SQ>) | [2] T_NONE
$HOME

echo "test\"quote"
[0] T_WORD (echo) | [1] T_WORD (<DQ>test"quote<DQ>) | [2] T_NONE
test"quote

echo 'can'\''t'
[0] T_WORD (echo) | [1] T_WORD (<SQ>can<SQ>'<SQ>t<SQ>) | [2] T_NONE
can't

```

## Parser

### sintassi

```
line := pipeline

pipeline := cmd {T_PIPE cmd};

cmd := item {item};

item := T_WORD
		| T_RED_OUT		T_WORD
		| T_RED_OUT_APP	T_WORD
		| T_RED_IN		T_WORD
		| T_HEREDOC		T_WORD;
```

#### Note

La linea non può terminare con una pipe
Un comando (cmd) deve avere almeno il nome del comando o una redirection
Una redirection non seguita da una parola dà errore di sintassi
Le redirection possono essere sovrascritte (es: name > out > out1)
Un comando con solo redirection è consentito, crea i file delle redirection e termina
Nel parser non vengono creati file o heredoc

### Parsing

seguendo com'è definita la sintassi, il parser è implementato usando una funzione per ogni blocco

input: lista dei token
output: lista dei comandi

Ogni comando è formato da:
- argc e argv per gli argomenti
- una lista di redirection, tutti i file verranno creati / aperti, ma solo l'ultimo sarà utilizzato come fdin/fdout del comando

Il parser mantiene nello stato solo il token corrente (current_token) e la pipeline che sta generando

## Expander

Si occupa di espandere le variabili d'ambiente, sia negli args del comando che nelle redirections

gestisce:
\$KEY -> valore da getenv()
\$? -> exit code dell'ultimo comando
~ -> equivalente a \$HOME se messa all'inizio del token e \$HOME è settato

vengono gestite le quotes:
se si è all'interno di single quotes, non vi è espansione

note:
nelle redirection non è gestito ambiguous redirect per semplicità
il mark MARK_DQ non viene usato ma rimane utile per il debug nelle print

## Heredoc

Unica redirection che viene eseguita prima dell'esecuzione, per averla nel processo padre
Viene preso l'input fino al delimitatore e scritto in una pipe, che poi diventerà l'input del comando

## Executor

Executor_exe:
Prende in input la pipeline e la esegue

### Executor
- jobs -> tabella dei jobs
- process_map -> hash map che mappa i pid all'index del job
- interactive -> serve per disattivare alcune funzionalità nel caso in cui la shell non abbia accesso al terminale
- tty_fd -> file descriptor del terminale
- shell_pgid -> Process Group ID della shell

Ogni pipeline viene gestita come un job.
In ogni job ci sono più processi, che rappresentano i singoli comandi, che avranno un loro pid e un process group id comune.

Nel job viene salvato solo il pid dell'ultimo processo, utile per salvare il codice di uscita.
Nella tabella dei processi invece vengono salvati tutti i pid dei processi e mappati ai job.

### Logica

Se c'è un solo comando, ed è builtin (anche solo redirections), allora il comando viene avviato nel processo della shell (_Executor_parent).
In tutti gli altri casi, ogni comando all'interno della pipeline viene eseguito in un nuovo processo.

- si creano il numero di pipe giuste in base alla lunghezza della pipeline
- si crea un nuovo Job che viene aggiunto alla tabella dei jobs
- si itera per ogni comando nella pipeline:
	- vengono resettati i segnali
	- si crea un nuovo processo figlio dove viene eseguito il comando (_Executor_child)
	- nel processo della shell si prende il pid del primo comando e lo si imposta come pgid di tutti i comandi.
	- si aggiunge il processo al job e alla tabella dei processi
- si chiudono le pipe (nel processo della shell)
- si lascia l'accesso al terminale al job
- si attende la terminazione del job (Executor_wait_job)
- la shell riprende l'accesso del terminale

_Executor_parent:
Serve per avviare un comando builtin nel processo della shell.
- salva i fd dell'i/o
- applica le redirections
- esegue il comando
- ripristina l'i/o

_Executor_child:
viene richiamato nel processo figlio per eseguire un comando
- resetta i segnali
- imposta il group id (setpgid)
- imposta le pipe (dup2)
- applica le redirections
	- scorre la lista delle redirections del comando
	- per ognuna crea se necessario il file e lo apre
	- alla fine applica l'ultima redirection
- se è una builtin la avvia
- altrimenti esegue il comando tramite execvp

Executor_wait_job:
Serve per mettersi in attesa della terminazione di un job
- imposta i segnali
- finchè ci sono processi attivi:
	- attesa di un figlio nel gruppo (waitpid con WUNTRACED per ritornare anche quando il processo viene fermato)
	- se il processo è stato fermato, aggiorna le informazioni del job 
	- se il processo è terminato, lo si rimuove dalla tabella e se è l'ultimo si imposta lo status
- se non ci sono altri processi attivi, si imposta il job come DONE e si aggiorna l'exit code


## Job Control

fg / bg / jobs
https://afni.nimh.nih.gov/pub/dist/edu/data/CD.expanded/AFNI_data6/unix_tutorial/misc/unix_commands.html
https://www.redhat.com/en/blog/jobs-bg-fg

https://copyconstruct.medium.com/bash-job-control-4a36da3e4aa7

La gestione dei jobs è una feature avanzata presente in bash. Permette di mettere in pausa, riprendere e di avviare in background i Job in esecuzione.

Un singolo comando è eseguito in un processo separato.
Una sequenza di comandi separata da pipe è detta pipeline e viene gestita con un Job.
Un Job può essere in RUNNING, STOPPED o DONE.
I processi della stessa pipeline condividono lo stesso process group id, che sarà uguale al pid del primo comando.
Un processo può essere in esecuzione in background, quindi in RUNNING ma senza avere accesso alla tty.
Tutti i jobs sono salvati all'interno di una Table.
Viene anche mantenuto il job corrente secondo questa logica (JobsTable_update_current):
E' l'ultimo job che è stato stoppato (STOPPED). Se non ci sono stopped è il job che è stato avviato meno recentemente (RUNNING). (come zsh)
Inoltre per facilitare la ricerca, viene mantenuta una hash map in cui sono mappati i pid dei singoli comandi al job di cui fanno parte (in particolare all'indice nella tabella dei jobs).

Per mettere in pausa (STOPPED) un job, basta premere CTRL+Z dopo averlo avviato (SIGTSTP).

### Builtin

```jobs [ids]```

Permette di visualizzare la tabella dei jobs, con: id, job corrente, stato e la stringa che rappresenta la pipeline.
Se si passano degli ids, vengono cercati nella job table e poi stampati a schermo i job corrispondenti.


```fg [id]```

Permette di riportare un job in foreground.
Se non viene passato un id, viene usato il current job.
- si dà l'accesso della tty al job (tcsetpgrp)
- se il job è stopped, si invia il segnale SIGCONT per notificare il kernel e far riprendere l'esecuzione del job sospeso
- ci si mette in wait del job (Executor_wait_job)
- infine si ridà l'accesso del tty alla shell


```bg [id]```

Permette di far riprendere l'esecuzione in background di un job stopped
Se non viene passato un id, viene usato il current job.

- si invia il segnale SIGCONT per notificare il kernel e far riprendere l'esecuzione del job sospeso

#### Update Job state

Per aggiornare lo stato dei job nel caso in cui un job in background termini, prima e dopo readline viene richiamata una routine che si mette in wait di aggiornamenti dei processi figli.
Si usa waitpid con le seguenti flags:
- `WNOHANG`: per avere un comportamento non bloccante
- `WUNTRACED`: se un figlio riceve SIGSTOP o SIGTSTP, il padre riceve la notifica
- `WCONTINUED`: segnala anche i figli che erano sospesi e sono stati ripresi (SIGCONT)

Per accedere al job tramite il pid restituito da waitpid, si usa l'hash table pid_tab, che mappa i pid ai job. Quindi si aggiorna lo stato del job.

#### Chiusura pulita

Per non avere processi figli che restano in esecuzione anche quando la shell viene chiusa viene richiamata la funzione Executor_destroy.
Per prima cosa invia a tutti i job il segnale SIGHUP.
Questo è un segnale storico unix, nei primi sistemi i terminali erano collegati tramite linee seriali, se l'utente chiudeva la connessione il kernel inviava SIGHUP al processo controllato da quel terminale.
Il comportamento di default nei sistemi odierni è di terminare il processo che lo riceve. Alcuni daemon invece lo implementano come segnale per ricaricare il file di configurazione.

Se viene intercettato e gestito diversamente, questo segnale non porta alla chiusura del processo. Per questo si invia in seguito il segnale SIGTERM, il più utilizzato e consigliato per chiudere in modo pulito un processo.
Infine per sicurezza si invia il segnale SIGKILL per forzare la chiusura del processo.

E' importante anche usare waitpid alla fine per non lasciare processi zombie in attesa.


## Builtins base

- `cd [path]`

Serve per cambiare directory a quella passata per argomento (con `chdir`). Senza argomenti viene usata la `$HOME`. Se viene passato `-` si usa `$OLDPWD`.
Infine vengono aggiornate le variabili `$OLDPWD` e `$PWD`.

- `echo [-n] [args]`

Stampa in console gli args. Se viene passato -n non va a capo alla fine della stringa.

- `exit [code]`

Chiude la shell, il valore di uscita sarà il codice passato se presente.

- `env`

Stampa tutte le variabili d'ambiente

- `export [key=value]`

Serve per impostare una variabile d'ambiente (usando `setenv`). Se non viene passato alcun argomento, ha lo stesso comportamento di `env`.

- `pwd`

Stampa la directory attuale (usando `getcwd`).

## Prompt

Appena avviata la shell, vengono mostrati il nome `Minishell CSO` e il livello della shell (SHLVL)

Il prompt mostra il nome della directory corrente ($PWD), il branch della repo git (se presente) e indica se l'ultimo comando è terminato con un codice diverso da 0, in base al colore delle `>>`.

Per ottenere il branch, si usa il comando `git branch --show-current 2>/dev/null`, avviato tramite `popen`, che apre una pipe stream con il processo avviato.  Il nome del branch viene letto con `fgets()` dalla pipe.

## TEST

- cd & pwd

```bash
cd /
pwd
cd -      # torna a OLDPWD
pwd
cd        # va in $HOME
```

- echo

```bash
echo ciao mondo
echo -n no-newline
echo "a  b" c      # spazi preservati in DQ
echo '$HOME'       # no espansione in SQ
```

- env

```bash
export FOO=bar
env | grep '^FOO='
unset FOO
env | grep '^FOO='
```

- exit

```bash
exit 7
echo $?
```

- lexer & expander

```bash
echo a\ \ b \| "a  b" \| 'a  b'
echo 'can'\''t'
echo "test\"quote"
echo $HOME "$HOME" '$HOME'

export X=1
echo $X
echo $?
echo ~ 
```

- redirections

```bash
echo hi > out
cat out
echo a >> out
cat < out
echo x > a > b # solo l’ultima deve contenere output
cat a
cat b 
```

- pipe

```bash
echo ciao | tr a-z A-Z
printf "a\nb\n" | grep b | wc -l
cat | cat | echo ciao
```

- heredoc

```bash
cat <<EOF
riga1
riga2
EOF
```

- job control

```bash
sleep 10
# Ctrl+Z
jobs            # deve mostrare STOPPED
fg              # riprende in foreground
```


```bash
sleep 3 | sleep 3
# Ctrl+Z
bg
jobs             # RUNNING
```

- Segnali e aggiornamento dei jobs

```bash
yes > /dev/null
# Ctrl+Z
kill -CONT <pid>
jobs   # RUNNING
```

- Errori sintassi 

```bash
|
echo a |
echo >        # redirection senza parola
echo "unclosed
```
 
- Errore permessi

```bash
mkdir d
chmod -w d
echo x > d/f
```
