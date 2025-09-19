# minishell-cso

### TODO:

- [ ] fix error: give_terminal_to: Bad file descriptor
- [ ] add bg and fg
- [ ] add newline for ctr + c

### readline dep
sudo apt-get install libreadline-dev

## Lessico

spaziatori: spazi e tab (ad eccezioni di quelli nelle quotes)

quotes: '...' e "..." -> il contenuto viene copiato letteralmente

backslash: \char -> il char viene copiato letteralmente (\\ -> viene copiato \)
			Nelle quotes non sono un carattere speciale

Tipi di Token:
- T_WORD

- T_PIPE = |

- T_RED_OUT = >
- T_RED_OUT_APP = >>
- T_RED_IN = <
- T_HEREDOC = <<

Errori lessicali:
- Quotes non chiuse
- Nessun carattere dopo \
- Input eccede la lunghezza massima

## Sintassi

pipeline = cmd {| cmd}
una pipe non seguita da un comando da errore di sintassi
se ci sono troppi comandi, da errore di sintassi

cmd = {name} {args} {< in} {> out} {<< eof} {>> a_out}
args = arg1 arg2 ... argn
le redirections possono essere in qualsiasi posizione

una redirection non seguita da una parola da errore di sintassi
Le redirection possono essere sovrascritte (es: name > out > out1)
Simulando quello che succede in bash, sia out che out1 vengono creati
Un comando senza nome, ma solo con redirection, crea i file delle redirection e termina

line := pipeline

pipeline := cmd {T_PIPE cmd};			// non ci possono essere pipe alla fine

cmd := item {item};					// almeno un comando o una redirection
									// un comando con solo redirection è consentito

item := T_WORD
		| T_RED_OUT		T_WORD
		| T_RED_OUT_APP	T_WORD
		| T_RED_IN		T_WORD
		| T_HEREDOC		T_WORD;

## Parser
seguendo com'è definita la sintassi, il parser è implementato usando una funzione per ogni blocco
ne esce fuori la lista dei comandi, ogni comando ha:
argc e argv per gli argomenti
una lista di redirection, tutti i file verranno creati / aperti, ma solo l'ultimo sarà utilzzato come fdin/fdout del comando


## Expander

'$' -> '$'

'$?' -> last exit code
'~' -> $HOME se è il primo carattere e se $HOME è settato, altrimenti resta ~
\$VAR -> espande VAR

nelle redirection non è gestito ambiguous redirect per semplicità

## Heredoc

unica redirection che viene eseguita prima dell'esecuzione, per farla nel processo padre
Viene preso l'input fino al delimitatore e scritto in una pipe, che poi diventerà l'input del comando

## Executor

Idee:

Classe Executor, che contiene lo shell pgid, tty e interactive
Inoltre ha la jobs list
Ha il metodo init, execute(jobs), clear e altro?
Ha il last_pid e il reset degli fd, salvando quelli precedenti

Poi abbiamo da gestire i jobs, con la classe Jobs tengo:
- la lista dei processi
- il pgid
- controllo stato per dire se è in backgroud o meno
Ha dei metodi utili?

Poi c'è un processo, che ha 
- il comando (detatch dalla lista dei comandi esistente)
- il suo pid
- lo stato
Ha il metodo init
che riconosce se è una funzione builtin o no, imposta il puntatore a funzione da richiamare, per le builtin punta alle funzioni built in, altrimenti punta alla funzione che esegue un comando esterno
Poi metterei anche una flag che dice se va avviato sul processo padre, di base metterei che se è una funzione builtin e non ci sono altri comandi. Non voglio gestire l'& alla fine per ora

Ha il metodo execute, che richiama il metodo execute_redirections
Il processo ha anche gli attributi di fdin e fdout e i metodi setter che prima di impostarli chiudono l'fd precedente

All'interno di execute crea la pipe (se necessario, quindi se ha un comando successivo), salva la pipe di lettura nel processo successivo, fa la fork se necessario, esegue le redirections e poi esegue il comando (se c'è)

Ricordiamoci di chiudere sempre tutti gli fd e di ripristinare gli fd del processo padre se vengono cambiati. 
Una volta avviata la pipeline si devono aspettare i processi figli e aggiornare il codice di uscita con il codice di uscita dell'ultimo processo (Salvato in last pid) o almeno ho fatto così nella versione precedente.

### Builtin

- echo (-n) args
