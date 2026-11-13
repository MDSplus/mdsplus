"""
Device MDSplus per il sistema OMICRON MPD Suite: MCU2 con canali MPD 800 / UHF 800.

Struttura
---------
  MCU2 e' l'unico device. Contiene N_CH (= 8) canali generici .CH_01 ... .CH_08;
  ogni canale descrive un'unita' di acquisizione (MPD 800 o UHF 800) tramite il
  nodo TYPE. Tutta la logica e' contenuta nella classe MCU2.

  Tre azioni, eseguite nello stesso processo (lo stesso action server):
    INIT   (INIT_ACTION)  avvia il thread, che si collega alla MPD Suite,
                          elenca i canali effettivamente collegati, li associa
                          ai CH_nn configurati (vedi "Ricerca dei canali"),
                          applica la configurazione e la controlla rileggendola
                          dalla Suite. Il device resta "ARMED": lo scope e'
                          configurato ma non si registra nulla.
    START  (START_ACTION) abilita gli snapshot e avvia la registrazione dei
                          burst. I burst avvenuti prima dello START sono scartati.
    STOP   (STOP_ACTION)  ferma la registrazione, scarica i buffer e scrive le
                          statistiche. Funziona anche dopo il solo INIT
                          (chiude la connessione senza dati).
  Il thread e' l'unico proprietario della connessione COM.

Configurazione (nel modello, shot -1, prima della campagna)
  MCU2
    :SERIAL      seriale della MCU (facoltativo, solo controllo)
    :REC_FOLDER  cartella base Windows o UNC per il dataset nativo (obbligatoria).
                 INIT crea <REC_FOLDER>/<anno>/<AAAAMMGG>, data locale del PC.
                 Prefisso file = <tree>_shot_<shot>; la Suite aggiunge data/ora.
                 START avvia dataset e snapshot; STOP attende la chiusura.
    :ACQ_MODE    POLL (verificato) | EVENT
    :SEG_ROWS    burst tenuti in memoria prima di scriverli
    :FLUSH_S     scrittura comunque ogni N s (ogni burst resta un segmento)
    :INIT_TMO    attesa massima della Suite [s]
    :T0_EXP      zero dei tempi dell'esperimento centrale, ns UTC dall'epoch
                 Unix, INTERO (int64). Di norma un'espressione su altri nodi
                 del tree; un valore float (o in secondi) perde la precisione
                 dei campioni: convertire con QUADWORD(...) se serve.
                 Vuoto -> BURSTS_REL / TIME_REL danno errore in lettura.
  .CH_nn
    :TYPE        'MPD800' | 'UHF800' | 'AUTO' | vuoto (= canale non usato)
    :LABEL       nome libero del sensore
    :ENABLED     0 = canale saltato anche se TYPE e' impostato
    :SERIAL      seriale dell'unita' (facoltativo: vuoto = assegnazione automatica)
    :CHANNEL     indice del canale nell'unita' (= topology[2]; usato solo con SERIAL)
    :TRIG_MODE   AUTO EDGE PEAK PD_EVENT ALWAYS NEVER PD_EVENT_WINDOW PASSIVE
    :TRIG_SLOPE  FALLING_EDGE NEGATIVE_PEAK RISING_EDGE POSITIVE_PEAK
                 EITHER_EDGE EITHER_PEAK
    :TRIG_THR    V per MPD800, dBm per UHF800
    :PRE_TRIG    [s]            :SCOPE_LEN  campioni per burst (0 ... 16384)
    :PD_BW :PD_FC   filtro PD [Hz]            (solo MPD800, ignorati altrimenti)
    :RX_FREQ        frequenza ricevitore [Hz] (solo UHF800, ignorato altrimenti)

  Ricerca dei canali (all'INIT)
    Il thread legge dalla Suite l'elenco dei canali collegati e validi
    (seriale, MCU, topologia [bus, posizione, canale], nome) e ne ricava il
    tipo di unita': dal nome predefinito della Suite ("MPD800 1.1.1",
    "UHF800 ...") o, se il canale e' stato rinominato, dai modi/slope di
    trigger supportati (PASSIVE / EITHER_* solo MPD800, PD_EVENT solo MPD600,
    ALWAYS / NEVER / PD_EVENT_WINDOW MPD800 e UHF800). L'elenco viene scritto
    in MCU2:INVENTORY. Poi associa i CH_nn:
      - SERIAL impostato: si cerca l'unita' (SERIAL, CHANNEL). Se non e'
        collegata, o e' su un'altra MCU, il CH_nn viene SALTATO con un avviso.
      - SERIAL vuoto: al CH_nn viene assegnato il primo canale collegato non
        ancora usato (ordine di topologia), del tipo TYPE (o di qualunque tipo
        supportato se TYPE = AUTO).
      - TYPE diverso dal tipo rilevato: vale il tipo rilevato (avviso) e i
        parametri che dipendono dal tipo (TRIG_THR, PD_BW, PD_FC, RX_FREQ) non
        vengono applicati, perche' le loro unita' cambiano col tipo.
      - tipo non rilevabile: si usa TYPE del tree (se e' AUTO, CH_nn saltato).
    L'unita' effettivamente usata e' scritta in CH_nn:DETECTED. L'INIT fallisce
    solo se nessun CH_nn trova un canale collegato.

  Controllo della configurazione (all'INIT)
    Dopo aver applicato le impostazioni di tutti i canali, il thread rilegge
    dalla Suite ogni parametro e lo confronta con quello richiesto nel tree
    (i float entro le tolleranze di MCU2.CHECK_TOL). La rilettura viene
    ripetuta per al massimo SETTLE_MAX_S secondi, per dare alla Suite il tempo
    di propagare le impostazioni.
    Ogni differenza e' un AVVISO, non un errore: viene stampata sullo standard
    output dell'action server e riportata in MCU2:STATUS, ma l'INIT prosegue e
    il device resta ARMED. Restano errori (INIT fallito) solo le configurazioni
    non valide nel tree (TYPE, TRIG_MODE, TRIG_SLOPE sconosciuti, nessun
    canale configurato collegato...).
    Nodo vuoto -> il parametro nella Suite non viene toccato ne' controllato;
    il valore attivo nella Suite viene solo riportato nel log.
    Nessun valore riletto viene salvato nel tree (salvo l'identita'
    dell'hardware in INVENTORY e DETECTED).

Dati (per canale, scritti nello shot)
  :DETECTED     unita' usata: tipo, seriale, MCU, topologia, nome nella Suite
  :BURSTS      segnale 1D (V per MPD, dBm per UHF): tutti i campioni in
                fila, ognuno col proprio tempo assoluto. Ogni burst e' UN
                segmento MDSplus (start = primo campione, end = ultimo
                campione, dim = Range con il passo di campionamento), quindi
                tra un burst e il successivo il tempo fa un salto. Si legge e
                si plotta come una forma d'onda normale; il singolo burst si
                legge con getSegment(i).
                Tempi = int64, ns UTC dall'epoch Unix, da scopeDataTime
                (orologio dello strumento, inizio dei dati del burst).
  :BURSTS_REL   gli stessi campioni di :BURSTS, con asse dei tempi :TIME_REL.
                E' un'espressione scritta nel modello all'aggiunta del device
                (BUILD_SIGNAL(DATA(BURSTS), *, TIME_REL)): non copia dati e
                si valuta in lettura, quindi T0_EXP puo' cambiare dopo lo shot.
                Non e' segmentato: per il singolo burst usare :BURSTS.
  :TIME_REL     asse dei tempi relativo all'esperimento [s], di default
                (DIM_OF(BURSTS) - MCU2:T0_EXP) * 1D-9 (vedi MCU2.REL_TIME_EXPR).
                Si puo' sostituire nel modello con qualunque espressione che
                dia un tempo per ogni campione di :BURSTS.
  :RESOLUTION   passo di campionamento dei burst [s] (scopeResolution)
  :TRIG_OFFSET  un valore per burst, scopeTriggerTimeOffset [s], con timestamp
                = tempo del primo campione del burst (segmenti di piu' burst)
  :N_BURSTS, :GAP_MEDIAN, :GAP_MAX   statistiche scritte allo STOP

Installazione
  - il file va in una cartella di MDS_PYDEVICE_PATH sia sul PC Windows (dove
    girano i metodi) sia sul server Linux (per aggiungere il device al tree).
    pywin32 viene importato solo nel thread, quindi il modulo si carica anche
    su Linux. Nessun altro file .py in quelle cartelle deve definire MCU2.
  - le azioni sono indirizzate al server 'MPD_SERVER': sostituiscilo con
    host:porta dell'action server sul PC Windows (es. 'mpdpc:8100').
  - fasi di dispatch: INIT, PULSE_ON (START) e STORE (STOP). Se la tua tabella
    delle fasi usa nomi diversi, cambiali nei valueExpr o nei nodi del modello.
  - cambiare N_CH o l'elenco dei nodi cambia la struttura: i tree creati con
    una versione diversa vanno ricreati (rimuovere e riaggiungere il device).
"""

# Solo gli import restano a livello di modulo.
import threading
import time
import traceback
from datetime import datetime, timezone, date

import numpy as np
from MDSplus import (Device, Tree, Float32Array, Float64Array,
                     Int64, Int64Array, Range)


class MCU2(Device):
    """MCU2 OMICRON con N_CH canali MPD800/UHF800 (vedi docstring del modulo)."""

    # ------------------------------------------------------------------
    # Parti del device
    # ------------------------------------------------------------------

    N_CH = 8

    ns = ('no_write_shot',)       # configurazione: si imposta nel modello
    nm = ('no_write_model',)      # stato / dati: scritti nello shot
    parts = [
        {'path': ':COMMENT',  'type': 'text'},
        {'path': ':SERIAL',   'type': 'text',    'options': ns},
        {'path': ':ACQ_MODE', 'type': 'text',    'value': 'POLL', 'options': ns},
        {'path': ':REC_FOLDER', 'type': 'text', 'options': ns},
        {'path': ':SEG_ROWS', 'type': 'numeric', 'value': 200,    'options': ns},
        {'path': ':FLUSH_S',  'type': 'numeric', 'value': 1.0,    'options': ns},
        {'path': ':INIT_TMO', 'type': 'numeric', 'value': 30.0,   'options': ns},
        # scrivibile anche nello shot: lo zero si puo' correggere a posteriori
        {'path': ':T0_EXP',   'type': 'numeric'},
        {'path': ':T0_UTC',    'type': 'numeric', 'options': nm},
        {'path': ':START_UTC', 'type': 'text',    'options': nm},
        {'path': ':STOP_UTC',  'type': 'text',    'options': nm},
        {'path': ':STATUS',    'type': 'text',    'options': nm},
        {'path': ':INVENTORY', 'type': 'text',    'options': nm},
        {'path': ':INIT_ACTION', 'type': 'action', 'options': ns,
         'valueExpr': "Action(Dispatch('MPD_SERVER','INIT',50,None),Method(None,'init',head))"},
        {'path': ':START_ACTION', 'type': 'action', 'options': ns,
         'valueExpr': "Action(Dispatch('MPD_SERVER','PULSE_ON',50,None),Method(None,'start',head))"},
        {'path': ':STOP_ACTION', 'type': 'action', 'options': ns,
         'valueExpr': "Action(Dispatch('MPD_SERVER','STORE',50,None),Method(None,'stop',head))"},
    ]
    # Asse dei tempi relativo all'esperimento centrale, per :TIME_REL di ogni
    # canale ($1 = :BURSTS del canale, $2 = MCU2:T0_EXP). La sottrazione resta
    # in int64 (ns) e solo il risultato passa a float64 [s]: 1D-9 e' double,
    # 1E-9 sarebbe float32 e perderebbe la risoluzione dei campioni.
    REL_TIME_EXPR = '(DIM_OF($1) - $2) * 1D-9'

    # ciclo esplicito (non comprehension: nel corpo di classe non vedrebbe ns/nm)
    for _i in range(1, N_CH + 1):
        _c = '.CH_%02d' % _i
        parts += [
            {'path': _c, 'type': 'structure'},
            {'path': _c + ':TYPE',       'type': 'text',    'options': ns},
            {'path': _c + ':LABEL',      'type': 'text',    'options': ns},
            {'path': _c + ':ENABLED',    'type': 'numeric', 'value': 1,             'options': ns},
            {'path': _c + ':SERIAL',     'type': 'text',    'options': ns},
            {'path': _c + ':CHANNEL',    'type': 'numeric', 'value': 1,             'options': ns},
            {'path': _c + ':TRIG_MODE',  'type': 'text',    'value': 'EDGE',        'options': ns},
            {'path': _c + ':TRIG_SLOPE', 'type': 'text',    'value': 'RISING_EDGE', 'options': ns},
            {'path': _c + ':TRIG_THR',   'type': 'numeric', 'options': ns},
            {'path': _c + ':PRE_TRIG',   'type': 'numeric', 'options': ns},
            {'path': _c + ':SCOPE_LEN',  'type': 'numeric', 'value': 16384,         'options': ns},
            {'path': _c + ':PD_BW',      'type': 'numeric', 'options': ns},
            {'path': _c + ':PD_FC',      'type': 'numeric', 'options': ns},
            {'path': _c + ':RX_FREQ',    'type': 'numeric', 'options': ns},
            {'path': _c + ':DETECTED',    'type': 'text',    'options': nm},
            {'path': _c + ':RESOLUTION',  'type': 'numeric', 'options': nm},
            {'path': _c + ':BURSTS',      'type': 'signal',  'options': nm},
            # espressioni definite nel modello: nessun dato copiato, valutate in lettura
            {'path': _c + ':TIME_REL',    'type': 'numeric', 'options': ns,
             'valueExpr': "Data.compile(%r, head.getNode('%s:BURSTS'), head.getNode(':T0_EXP'))"
                          % (REL_TIME_EXPR, _c)},
            {'path': _c + ':BURSTS_REL',  'type': 'signal',  'options': ns,
             'valueExpr': "Data.compile('BUILD_SIGNAL(DATA($1), *, $2)', "
                          "head.getNode('%s:BURSTS'), head.getNode('%s:TIME_REL'))" % (_c, _c)},
            {'path': _c + ':TRIG_OFFSET', 'type': 'signal',  'options': nm},
            {'path': _c + ':N_BURSTS',    'type': 'numeric', 'options': nm},
            {'path': _c + ':GAP_MEDIAN',  'type': 'numeric', 'options': nm},
            {'path': _c + ':GAP_MAX',     'type': 'numeric', 'options': nm},
        ]
    del _i, _c, ns, nm

    # ------------------------------------------------------------------
    # Eccezioni
    # ------------------------------------------------------------------

    try:
        from MDSplus.mdsExceptions import DevBAD_PARAMETER, DevCOMM_ERROR, DevINV_SETUP
    except Exception:                               # pragma: no cover
        class DevBAD_PARAMETER(Exception):
            pass

        class DevCOMM_ERROR(Exception):
            pass

        class DevINV_SETUP(Exception):
            pass

    class MpdConfigError(RuntimeError):
        """Errore di configurazione rilevato nel thread: diventa DevINV_SETUP."""

    # ------------------------------------------------------------------
    # Costanti dell'API MPD Suite (valori dell'enum dalla documentazione)
    # ------------------------------------------------------------------

    TRIGGER_MODES = {
        'AUTO': 0, 'EDGE': 1, 'PEAK': 2, 'PD_EVENT': 3, 'ALWAYS': 4,
        'NEVER': 5, 'PD_EVENT_WINDOW': 6, 'PASSIVE': 7,
    }
    TRIGGER_SLOPES = {
        'FALLING_EDGE': 0, 'NEGATIVE_PEAK': 1, 'RISING_EDGE': 2,
        'POSITIVE_PEAK': 3, 'EITHER_EDGE': 4, 'EITHER_PEAK': 5,
    }
    # -1 = UNKNOWN (per la slope: "la slope non si applica al modo attivo")
    MODE_NAMES = dict([(v, k) for k, v in TRIGGER_MODES.items()] + [(-1, 'UNKNOWN')])
    SLOPE_NAMES = dict([(v, k) for k, v in TRIGGER_SLOPES.items()] + [(-1, 'UNKNOWN')])

    MPD_PROGID = "mpd.MPDSoftware"
    EPOCH_2002_NS = 1_009_843_200 * 1_000_000_000      # 2002-01-01 UTC in ns Unix

    # ------------------------------------------------------------------
    # Controllo richiesto / riletto
    # ------------------------------------------------------------------

    # Tolleranze per i parametri float: (assoluta, relativa). Il controllo passa
    # se |riletto - richiesto| <= max(assoluta, relativa * |richiesto|).
    # La Suite puo' arrotondare i valori alla risoluzione dello strumento: se
    # compaiono avvisi dovuti solo all'arrotondamento, allarga la voce relativa.
    CHECK_TOL = {
        'TRIG_THR_V':   (1e-3, 1e-3),   # V
        'TRIG_THR_DBM': (0.1, 0.0),     # dB
        'PRE_TRIG':     (8e-9, 1e-3),   # s (8 ns = un campione a 125 MS/s)
        'PD_BW':        (1.0, 1e-3),    # Hz
        'PD_FC':        (1.0, 1e-3),    # Hz
        'RX_FREQ':      (1.0, 1e-4),    # Hz
    }
    SETTLE_MIN_S = 0.3      # attesa minima prima di accettare la rilettura
    SETTLE_MAX_S = 2.0      # oltre, le differenze rimaste diventano avvisi

    # Registro dei thread attivi, condiviso da tutte le istanze
    # (INIT e STOP vengono eseguiti su due istanze diverse del device).
    _WORKERS = {}
    _WORKERS_LOCK = threading.Lock()

    # ------------------------------------------------------------------
    # Utilita'
    # ------------------------------------------------------------------

    @staticmethod
    def _raise(exc_cls, msg):
        print('[MPD] ' + msg)
        try:
            exc = exc_cls(msg)
        except Exception:
            exc = exc_cls()
        raise exc

    @staticmethod
    def _val(node, default=None):
        try:
            return node.data()
        except Exception:
            return default

    @staticmethod
    def _text(node, default=None):
        v = MCU2._val(node)
        if v is None:
            return default
        if isinstance(v, bytes):
            v = v.decode()
        v = str(v).strip()
        return v if v else default

    @staticmethod
    def _num(node, default=None):
        v = MCU2._val(node)
        if v is None:
            return default
        try:
            return float(np.asarray(v).ravel()[0])
        except Exception:
            return default

    @staticmethod
    def _tree_name(tree):
        return getattr(tree, 'tree', None) or getattr(tree, 'name')

    @staticmethod
    def _ticks_to_ns(ticks):
        """scopeDataTime (tick da 125 ps dal 2002-01-01 UTC) -> ns UTC Unix."""
        return MCU2.EPOCH_2002_NS + (int(ticks) * 125) // 1000

    @staticmethod
    def _utc_now():
        return datetime.now(timezone.utc).isoformat()

    @staticmethod
    def _cast(obj, iface):
        """CastTo tollerante: nel tuo script funziona con obj._dispobj_."""
        import win32com.client
        for target in (getattr(obj, '_dispobj_', None), obj):
            if target is None:
                continue
            try:
                return win32com.client.CastTo(target, iface)
            except Exception:
                continue
        raise MCU2.MpdConfigError("impossibile ottenere l'interfaccia %s" % iface)

    @staticmethod
    def _detect_type(comch, name):
        """Tipo di unita' di un canale della Suite: (tipo, come) con tipo
        'MPD800' | 'UHF800' | 'MPD600' | None. L'API non ha una proprieta'
        "tipo": si usa il nome predefinito della Suite ("MPD800 1.1.1") e, se
        il canale e' stato rinominato, i modi/slope di trigger supportati."""
        key = name.upper().replace(' ', '')
        for typ in ('MPD800', 'UHF800', 'MPD600'):
            if key.startswith(typ):
                return typ, 'nome'
        try:
            modes = set(int(x) for x in comch.scope.supportedTriggerModes)
            slopes = set(int(x) for x in comch.scope.supportedTriggerSlopes)
        except Exception:
            return None, 'non rilevabile'
        T, S = MCU2.TRIGGER_MODES, MCU2.TRIGGER_SLOPES
        if T['PASSIVE'] in modes or slopes & {S['EITHER_EDGE'], S['EITHER_PEAK']}:
            return 'MPD800', 'trigger supportati'
        if T['PD_EVENT'] in modes:
            return 'MPD600', 'trigger supportati'
        if modes & {T['ALWAYS'], T['NEVER'], T['PD_EVENT_WINDOW']}:
            return 'UHF800', 'trigger supportati'
        return None, 'non rilevabile'

    @staticmethod
    def _describe(unit):
        return '%s SN %s MCU %s topologia %s nome "%s" (tipo da %s)' % (
            unit['type'] or '?', unit['serial'], unit['mcu'] or '?',
            '.'.join(str(x) for x in unit['topo']), unit['name'], unit['how'])

    # ---- confronto di un parametro (vedi _Mcu2Worker._apply / _verify) ----

    @staticmethod
    def _fmt(chk, v):
        if v is None:
            return '?'
        kind = chk['kind']
        if kind == 'enum':
            return chk['names'].get(int(v), str(v))
        if kind == 'bool':
            return 'si' if v else 'no'
        if kind == 'int':
            return '%d' % int(v)
        return ('%.10g %s' % (v, chk['unit'])).strip()

    @staticmethod
    def _compare(chk):
        """'OK' | 'DIFF' | 'ERR' (rilettura fallita) | 'NA' (non applicabile)."""
        if chk['err'] is not None or chk['got'] is None:
            return 'ERR'
        want, got = chk['want'], chk['got']
        if chk['na'] is not None and got == chk['na']:
            return 'NA'
        if chk['kind'] == 'float':
            tol_abs, tol_rel = chk['tol']
            ok = abs(float(got) - float(want)) <= max(tol_abs, tol_rel * abs(float(want)))
        elif chk['kind'] == 'bool':
            ok = bool(got) == bool(want)
        else:
            ok = int(got) == int(want)
        return 'OK' if ok else 'DIFF'

    # ------------------------------------------------------------------
    # Driver per tipo di unita' (scelto dal nodo TYPE del canale)
    # ------------------------------------------------------------------

    class _UnitDriver(object):
        THR_UNIT = 'V'
        THR_TOL = 'TRIG_THR_V'          # chiave in MCU2.CHECK_TOL

        @staticmethod
        def _set_threshold(scope, value):
            scope.setTriggerThreshold(float(value))

        @staticmethod
        def _get_threshold(scope):
            return float(scope.triggerThreshold)

        # Default VOLUTAMENTE vuoto: ridefinito dai driver specifici.
        @staticmethod
        def _specific(comch, head, warn):
            """Parametri propri del tipo di unita', in ordine di applicazione:
            lista di (nodo, setter, getter, unita', chiave di CHECK_TOL)."""
            return []

    class _Mpd800Driver(_UnitDriver):
        """MPD 800. TRIG_THR in V. PD_BW / PD_FC: filtro PD [Hz]."""
        THR_UNIT = 'V'
        THR_TOL = 'TRIG_THR_V'

        @staticmethod
        def _specific(comch, head, warn):
            pd = comch.pd
            bw = MCU2._num(head.getNode(':PD_BW'))
            if bw is not None:
                # le bande del filtro PD sono discrete (supportedBandwidths)
                try:
                    supp = [float(x) for x in pd.supportedBandwidths]
                except Exception:
                    supp = []
                if supp and not any(abs(bw - s) <= max(1.0, 1e-3 * s) for s in supp):
                    warn('PD_BW %g Hz non e\' tra le bande supportate [%s]'
                         % (bw, ', '.join('%g' % s for s in supp)))
            # prima la banda: cambiandola la Suite puo' spostare la frequenza
            # centrale al minimo ammesso per la nuova banda
            return [
                (':PD_BW', pd.setBandwidth, lambda: float(pd.bandwidth), 'Hz', 'PD_BW'),
                (':PD_FC', pd.setCenterFrequency, lambda: float(pd.centerFrequency),
                 'Hz', 'PD_FC'),
            ]

    class _Uhf800Driver(_UnitDriver):
        """UHF 800. TRIG_THR in dBm. RX_FREQ: frequenza del ricevitore [Hz]."""
        THR_UNIT = 'dBm'
        THR_TOL = 'TRIG_THR_DBM'

        @staticmethod
        def _set_threshold(scope, value):
            scope.setTriggerThresholdDbm(float(value))

        @staticmethod
        def _get_threshold(scope):
            return float(scope.triggerThresholdDbm)

        @staticmethod
        def _specific(comch, head, warn):
            want = MCU2._num(head.getNode(':RX_FREQ'))
            try:
                uhf = MCU2._cast(comch, 'IMpdChannel2').uhf
            except MCU2.MpdConfigError as exc:
                if want is not None:
                    warn('RX_FREQ non impostabile: %s' % exc)
                return []
            if want is not None:
                try:
                    if uhf.sweepEnabled:
                        warn('sweep UHF attivo: la Suite ignora RX_FREQ durante lo sweep')
                except Exception:
                    pass
            return [(':RX_FREQ', uhf.setRxFrequency, lambda: float(uhf.rxFrequency),
                     'Hz', 'RX_FREQ')]

    UNIT_DRIVERS = {'MPD800': _Mpd800Driver, 'UHF800': _Uhf800Driver}

    # ------------------------------------------------------------------
    # Runtime di un canale durante l'acquisizione
    # ------------------------------------------------------------------

    class _UnitRuntime(object):
        """Buffer e scrittura di un canale. Ogni burst diventa UN segmento
        1D di :BURSTS, con il tempo assoluto di ogni campione (vedi
        _write_burst). I burst restano in RAM al massimo SEG_ROWS burst o
        FLUSH_S secondi prima di essere scritti."""

        def __init__(self, head, comch, snapshot, log):
            self.path = str(head.fullpath)
            self.head = head
            self.comch = comch
            self.snapshot = snapshot
            self.log = log
            self.bursts_node = head.getNode(':BURSTS')
            self.offs_node = head.getNode(':TRIG_OFFSET')
            self.res_node = head.getNode(':RESOLUTION')
            # per ogni burst in attesa: t0 [ns UTC], campioni, offset, passo [s]
            self.buf_t, self.buf_d, self.buf_o, self.buf_r = [], [], [], []
            self.nsamp = None
            self.res = None
            self.last_ticks = None
            self.prev_end_ns = None
            self.n_bursts = 0
            self.n_segments = 0
            self.gaps = []
            self.last_flush = time.monotonic()
            self.warned_float = False
            self.warned_dt = False
            self.warned_overlap = False

        def add(self, ticks, res, off, data):
            if not ticks:
                return
            if isinstance(ticks, float) and not self.warned_float:
                self.log('%s: scopeDataTime arriva come float, precisione persa' % self.path)
                self.warned_float = True
            ticks = int(ticks)
            if ticks == self.last_ticks:
                return
            data = np.asarray(data, dtype=np.float32).ravel()
            if data.size == 0 or res <= 0:
                return
            self.last_ticks = ticks

            # ogni burst e' un segmento a se': la lunghezza puo' cambiare
            if self.nsamp is not None and data.size != self.nsamp:
                self.log('%s: lunghezza burst cambiata %d -> %d'
                         % (self.path, self.nsamp, data.size))
            self.nsamp = data.size

            if self.res != res:
                if self.res is not None:
                    self.log('%s: risoluzione cambiata %g -> %g s' % (self.path, self.res, res))
                self.res = res
                self.res_node.record = float(res)

            t_ns = MCU2._ticks_to_ns(ticks)
            if self.prev_end_ns is not None:
                gap = (t_ns - self.prev_end_ns) * 1e-9
                if gap < 0 and not self.warned_overlap:
                    self.log('%s: burst sovrapposto al precedente di %g s: il tempo '
                             'del segnale non sara\' monotono' % (self.path, -gap))
                    self.warned_overlap = True
                self.gaps.append(gap)
            # fine del burst = tempo dell'ultimo campione + un passo
            self.prev_end_ns = t_ns + data.size * res * 1e9

            self.buf_t.append(t_ns)
            self.buf_d.append(data)
            self.buf_o.append(float(off))
            self.buf_r.append(float(res))
            self.n_bursts += 1

        def poll(self):
            """Lettura via property. Rilegge il timestamp per scartare letture
            a cavallo di due burst."""
            snap = self.snapshot
            ticks = snap.scopeDataTime
            if not ticks or int(ticks) == self.last_ticks:
                return
            data = snap.scopeData
            res = float(snap.scopeResolution)
            off = float(snap.scopeTriggerTimeOffset)
            if snap.scopeDataTime == ticks:
                self.add(ticks, res, off, data)

        def due(self, rows, flush_s):
            return bool(self.buf_t) and (
                len(self.buf_t) >= rows or time.monotonic() - self.last_flush >= flush_s)

        def flush(self):
            self.last_flush = time.monotonic()
            if not self.buf_t:
                return
            for t0, d, res in zip(self.buf_t, self.buf_d, self.buf_r):
                self._write_burst(int(t0), d, res)
            # TRIG_OFFSET ha un solo valore per burst: un segmento per flush
            self.offs_node.makeTimestampedSegment(
                Int64Array(np.asarray(self.buf_t, dtype=np.int64)),
                Float64Array(np.asarray(self.buf_o, dtype=np.float64)))
            self.buf_t, self.buf_d, self.buf_o, self.buf_r = [], [], [], []

        def _write_burst(self, t0, d, res):
            """Scrive UN burst come UN segmento 1D di :BURSTS.
            start = tempo del primo campione, end = tempo dell'ultimo, dim =
            tempo di ogni campione [ns UTC]. Se il passo e' un numero intero di
            ns si usa un Range (compatto), altrimenti la lista esplicita."""
            n = int(d.size)
            dt = res * 1e9                           # passo in ns
            if dt < 1.0 and not self.warned_dt:
                self.log('%s: passo di campionamento %g s < 1 ns: i tempi in ns '
                         'dei campioni vengono arrotondati' % (self.path, res))
                self.warned_dt = True
            dt_i = int(round(dt))
            if dt_i > 0 and abs(dt - dt_i) < 1e-6:
                t_end = t0 + dt_i * (n - 1)
                dim = Range(Int64(t0), Int64(t_end), Int64(dt_i))
            else:
                times = t0 + np.round(np.arange(n) * dt).astype(np.int64)
                t_end = int(times[-1])
                dim = Int64Array(times)
            self.bursts_node.makeSegment(Int64(t0), Int64(t_end), dim, Float32Array(d))
            self.n_segments += 1

        def write_stats(self):
            self.head.getNode(':N_BURSTS').record = int(self.n_bursts)
            if self.n_segments != self.n_bursts:
                self.log('%s: ATTENZIONE %d burst ma %d segmenti scritti'
                         % (self.path, self.n_bursts, self.n_segments))
            if self.gaps:
                g = np.asarray(self.gaps)
                self.head.getNode(':GAP_MEDIAN').record = float(np.median(g))
                self.head.getNode(':GAP_MAX').record = float(g.max())

    class _SnapshotEvents(object):
        """Gestore dell'evento scopeDataChanged (convenzione pywin32 "On"+evento)."""

        def bindRuntime(self, rt):
            self._rt = rt

        def OnscopeDataChanged(self, resolution, triggerTimeOffset, timestamp, data):
            self._rt.add(timestamp, float(resolution), float(triggerTimeOffset), data)

    # ------------------------------------------------------------------
    # Thread di acquisizione (unico proprietario di COM)
    # ------------------------------------------------------------------

    class _Mcu2Worker(threading.Thread):
        def __init__(self, tree_name, shot, mcu_path, cfg):
            super().__init__(name='MPD-%s' % mcu_path, daemon=True)
            self.tree_name = tree_name
            self.shot = shot
            self.mcu_path = mcu_path
            self.cfg = cfg
            self.ready = threading.Event()      # INIT completato (o fallito)
            self.start_evt = threading.Event()  # richiesta di START
            self.started = threading.Event()    # START completato (o fallito)
            self.stop_evt = threading.Event()   # richiesta di STOP
            self.recording = False
            self.error = None
            self.native_rec = None
            self.native_rec_owned = False
            self.native_filename = ''
            self.native_v2 = False
            self.messages = []
            self.warnings = []                  # differenze richiesto/riletto
            self.units = []

        def log(self, msg):
            line = '[MPD %s] %s' % (datetime.now().strftime('%H:%M:%S'), msg)
            print(line)
            self.messages.append(line)

        def warn(self, msg):
            """Avviso di configurazione: stampato subito e riportato in STATUS."""
            self.warnings.append(msg)
            self.log('ATTENZIONE: ' + msg)

        # ---------------- connessione e configurazione ----------------

        def _connect(self):
            import win32com.client
            app = win32com.client.gencache.EnsureDispatch(MCU2.MPD_PROGID)
            session = app.measurementSession
            t_end = time.monotonic() + self.cfg['init_tmo']
            while not (session.valid and session.initializationComplete):
                if time.monotonic() > t_end:
                    raise MCU2.MpdConfigError('MPD Suite non inizializzata entro %gs'
                                              % self.cfg['init_tmo'])
                self._pump()
                time.sleep(0.1)
            return app, session

        @staticmethod
        def _pump():
            import pythoncom
            pythoncom.PumpWaitingMessages()

        def _inventory(self, session):
            """Canali collegati e validi visti dalla Suite, in ordine di
            topologia (prima quelli della MCU attesa)."""
            M = MCU2
            exp_mcu = self.cfg['mcu_serial']
            inv = []
            for comch in list(session.channelCollection.channels):
                try:
                    if not comch.valid:
                        continue
                    serial = str(comch.serialNumber or '').strip()
                    topo = [int(x) for x in list(comch.topology)]
                    mcu = str(comch.mcuSerialNumber or '').strip()
                    name = str(comch.name or '').strip()
                except Exception as exc:
                    self.log('canale della Suite illeggibile, ignorato: %s' % exc)
                    continue
                typ, how = M._detect_type(comch, name)
                inv.append({'com': comch, 'serial': serial, 'topo': topo,
                            'index': topo[2] if len(topo) > 2 else 1,
                            'mcu': mcu, 'name': name, 'type': typ, 'how': how,
                            'other_mcu': bool(exp_mcu and mcu and mcu != exp_mcu)})
            inv.sort(key=lambda u: (u['other_mcu'], u['mcu'], u['topo']))
            return inv

        def _match(self, inv):
            """Associa i CH_nn configurati ai canali collegati: lista di
            (cfg del CH_nn, canale della Suite). I CH_nn senza un canale
            collegato vengono saltati con un avviso."""
            M = MCU2
            used, out = set(), []
            chans = self.cfg['channels']

            # prima i CH_nn con SERIAL: occupano la loro unita'
            for ch in (c for c in chans if c['serial']):
                hits = [u for u in inv
                        if u['serial'] == ch['serial'] and u['index'] == ch['index']]
                if not hits:
                    same = [str(u['index']) for u in inv if u['serial'] == ch['serial']]
                    self.warn('%s: unita\' %s canale %d non collegata%s: canale saltato'
                              % (ch['name'], ch['serial'], ch['index'],
                                 ' (di quell\'unita\' la Suite vede i canali %s)'
                                 % ', '.join(same) if same else ''))
                    continue
                u = hits[0]
                if u['other_mcu']:
                    self.warn('%s: unita\' %s collegata alla MCU %s, attesa %s: canale saltato'
                              % (ch['name'], ch['serial'], u['mcu'], self.cfg['mcu_serial']))
                    continue
                used.add(id(u))
                out.append((ch, u))

            # poi i CH_nn senza SERIAL: primo canale libero del tipo richiesto
            for ch in (c for c in chans if not c['serial']):
                cand = [u for u in inv
                        if id(u) not in used and not u['other_mcu']
                        and u['type'] in M.UNIT_DRIVERS
                        and ch['type'] in ('AUTO', u['type'])]
                if not cand:
                    self.warn('%s: nessun canale %s libero tra quelli collegati: canale saltato'
                              % (ch['name'], 'supportato' if ch['type'] == 'AUTO'
                                 else ch['type']))
                    continue
                u = cand[0]
                used.add(id(u))
                out.append((ch, u))
                self.log('%s: assegnato automaticamente a %s' % (ch['name'], M._describe(u)))

            free = [u for u in inv if id(u) not in used and not u['other_mcu']]
            if free:
                self.log('canali collegati non usati: %s'
                         % '; '.join(M._describe(u) for u in free))
            out.sort(key=lambda p: p[0]['node'])
            return out

        def _resolve_type(self, ch, unit):
            """Tipo da usare per il CH_nn: (tipo, typed) con typed=False se i
            parametri che dipendono dal tipo non vanno applicati; tipo None =
            CH_nn da saltare."""
            name, det = ch['name'], unit['type']
            if det is None:
                if ch['type'] == 'AUTO':
                    self.warn('%s: tipo di %s non rilevabile e TYPE = AUTO: canale saltato'
                              % (name, MCU2._describe(unit)))
                    return None, False
                self.log('%s: tipo non rilevabile, uso TYPE %s del tree' % (name, ch['type']))
                return ch['type'], True
            if det not in MCU2.UNIT_DRIVERS:
                self.warn('%s: %s non supportato da questo device: canale saltato'
                          % (name, MCU2._describe(unit)))
                return None, False
            if ch['type'] not in ('AUTO', det):
                self.warn('%s: TYPE %s ma collegato %s: uso %s e NON applico TRIG_THR, '
                          'PD_BW, PD_FC, RX_FREQ (unita\' diverse per tipo)'
                          % (name, ch['type'], MCU2._describe(unit), det))
                return det, False
            return det, True

        def _apply(self, head, cls, comch, name, typed=True):
            """Applica al canale le impostazioni del tree e restituisce la lista
            dei controlli da fare sulla rilettura (vedi _verify). Un nodo vuoto
            non viene applicato: 'want' resta None e il valore della Suite
            viene solo riportato nel log. typed=False: i parametri che
            dipendono dal tipo di unita' vengono trattati come vuoti."""
            M = MCU2
            scope = comch.scope
            scope3 = M._cast(scope, 'IChScopeOptions3')
            warn = lambda msg: self.warn('%s: %s' % (name, msg))      # noqa: E731
            checks = []

            def check(par, want, getter, kind='float', unit='', tol=None,
                      names=None, na=None):
                checks.append({'ch': name, 'par': par, 'want': want, 'get': getter,
                               'kind': kind, 'unit': unit,
                               'tol': M.CHECK_TOL.get(tol, (0.0, 0.0)),
                               'names': names, 'na': na, 'got': None, 'err': None})

            def setp(par, setter, value):
                # un'impostazione rifiutata e' un avviso: la rilettura dira'
                # quale valore e' rimasto attivo
                try:
                    setter(value)
                except Exception as exc:
                    warn('impostazione di %s = %s rifiutata dalla Suite: %s'
                         % (par, value, exc))

            # --- modo e slope del trigger (nomi validati: errore se sconosciuti)
            try:
                supp_modes = [int(x) for x in scope.supportedTriggerModes]
                supp_slopes = [int(x) for x in scope.supportedTriggerSlopes]
            except Exception:
                supp_modes, supp_slopes = None, None

            want_mode = None
            mode = M._text(head.getNode(':TRIG_MODE'))
            if mode:
                if mode.upper() not in M.TRIGGER_MODES:
                    raise M.MpdConfigError('%s: TRIG_MODE "%s" sconosciuto (validi: %s)'
                                           % (name, mode, ', '.join(M.TRIGGER_MODES)))
                want_mode = M.TRIGGER_MODES[mode.upper()]
                if supp_modes is not None and want_mode not in supp_modes:
                    warn('TRIG_MODE %s non e\' tra i modi supportati %s, provo comunque'
                         % (mode.upper(), [M.MODE_NAMES.get(m, m) for m in supp_modes]))
                setp('TRIG_MODE', scope.setTriggerMode, want_mode)
            check('TRIG_MODE', want_mode, lambda: int(scope.triggerMode),
                  'enum', names=M.MODE_NAMES)

            want_slope = None
            slope = M._text(head.getNode(':TRIG_SLOPE'))
            if slope:
                if slope.upper() not in M.TRIGGER_SLOPES:
                    raise M.MpdConfigError('%s: TRIG_SLOPE "%s" sconosciuta (valide: %s)'
                                           % (name, slope, ', '.join(M.TRIGGER_SLOPES)))
                want_slope = M.TRIGGER_SLOPES[slope.upper()]
                if supp_slopes is not None and want_slope not in supp_slopes:
                    warn('TRIG_SLOPE %s non e\' tra le slope supportate %s, provo comunque'
                         % (slope.upper(), [M.SLOPE_NAMES.get(s, s) for s in supp_slopes]))
                setp('TRIG_SLOPE', scope.setTriggerSlope, want_slope)
            # slope riletta UNKNOWN (-1) = non si applica al modo attivo (es. ALWAYS)
            check('TRIG_SLOPE', want_slope, lambda: int(scope.triggerSlope),
                  'enum', names=M.SLOPE_NAMES, na=-1)

            # --- parametri numerici dello scope
            thr = M._num(head.getNode(':TRIG_THR')) if typed else None
            if thr is not None:
                setp('TRIG_THR', lambda v: cls._set_threshold(scope, v), thr)
            check('TRIG_THR', thr, lambda: cls._get_threshold(scope),
                  unit=cls.THR_UNIT, tol=cls.THR_TOL)

            pre = M._num(head.getNode(':PRE_TRIG'))
            if pre is not None:
                setp('PRE_TRIG', scope.setPreTriggerTime, pre)
            check('PRE_TRIG', pre, lambda: float(scope.preTriggerTime),
                  unit='s', tol='PRE_TRIG')

            slen = M._num(head.getNode(':SCOPE_LEN'))
            if slen is not None:
                slen = int(slen)
                if not 0 <= slen <= 16384:
                    warn('SCOPE_LEN %d fuori dall\'intervallo 0 ... 16384' % slen)
                setp('SCOPE_LEN', scope3.setScopeLength, slen)
            check('SCOPE_LEN', slen, lambda: int(scope3.scopeLength), 'int')

            # --- parametri propri del tipo di unita'
            specific = cls._specific(comch, head, warn if typed else (lambda msg: None))
            for node, setter, getter, unit, tol in specific:
                par = node.lstrip(':')
                val = M._num(head.getNode(node)) if typed else None
                if val is not None:
                    setp(par, setter, val)
                check(par, val, getter, unit=unit, tol=tol)

            # --- abilitazione dello scope (senza, non arrivano burst)
            setp('scope enabled', scope.setEnabled, True)
            check('SCOPE_ENABLED', True, lambda: bool(scope.enabled), 'bool')

            self.log('%s (%s): impostazioni inviate' % (name, comch.name))
            return checks

        def _verify(self, checks):
            """Rilegge dalla Suite tutti i parametri e li confronta con quelli
            richiesti. Ripete la lettura finche' tutto coincide o finche'
            scade SETTLE_MAX_S; le differenze rimaste diventano avvisi."""
            M = MCU2
            t0 = time.monotonic()
            while True:
                self._pump()
                time.sleep(0.05)
                for c in checks:
                    try:
                        c['got'], c['err'] = c['get'](), None
                    except Exception as exc:
                        c['got'], c['err'] = None, str(exc)
                pending = [c for c in checks
                           if c['want'] is not None and M._compare(c) in ('DIFF', 'ERR')]
                elapsed = time.monotonic() - t0
                if elapsed >= M.SETTLE_MIN_S and (not pending or elapsed >= M.SETTLE_MAX_S):
                    break

            channels = []
            for c in checks:
                if c['ch'] not in channels:
                    channels.append(c['ch'])
            for ch in channels:
                ok, notes = [], []
                for c in (c for c in checks if c['ch'] == ch):
                    par = c['par']
                    if c['want'] is None:
                        got = ('lettura fallita: ' + c['err']) if c['err'] else M._fmt(c, c['got'])
                        notes.append('%s non impostato (Suite: %s)' % (par, got))
                        continue
                    res = M._compare(c)
                    if res == 'OK':
                        ok.append('%s=%s' % (par, M._fmt(c, c['got'])))
                    elif res == 'NA':
                        notes.append('%s non applicabile al modo attivo' % par)
                    elif res == 'ERR':
                        self.warn('%s: %s richiesto %s, rilettura fallita: %s'
                                  % (ch, par, M._fmt(c, c['want']), c['err']))
                    else:
                        self.warn('%s: %s richiesto %s, la Suite riporta %s'
                                  % (ch, par, M._fmt(c, c['want']), M._fmt(c, c['got'])))
                if ok:
                    self.log('%s: verificati %s' % (ch, ', '.join(ok)))
                if notes:
                    self.log('%s: %s' % (ch, '; '.join(notes)))

        def _open_snapshot(self, scope, rt_holder):
            import win32com.client
            snap = scope.snapshot()
            if self.cfg['acq_mode'] == 'EVENT':
                snap = win32com.client.DispatchWithEvents(snap, MCU2._SnapshotEvents)
                snap.bindRuntime(rt_holder)
            snap.setEnabled(True)
            if not snap.valid:
                raise MCU2.MpdConfigError('snapshot non valido')
            # il burst gia' presente nello snapshot e' precedente allo START:
            # lo si marca come "gia' visto" perche' non venga registrato
            try:
                t0 = snap.scopeDataTime
                if t0:
                    rt_holder.last_ticks = int(t0)
            except Exception:
                pass
            return snap

        # Dataset recording remains entirely in this COM worker.
        NATIVE_START_TIMEOUT = 8.0
        NATIVE_STOP_TIMEOUT = 30.0

        @staticmethod
        def _optional_property(obj, name, default=None):
            """Only suppress missing COM members, never communication errors."""
            try:
                return getattr(obj, name)
            except AttributeError:
                return default
            except Exception as exc:
                # DISP_E_MEMBERNOTFOUND / DISP_E_UNKNOWNNAME (signed HRESULTs).
                if getattr(exc, 'hresult', None) in (-2147352573, -2147352570):
                    return default
                raise

        def _native_error(self):
            rec = self.native_rec
            if rec.errorCondition:
                raise MCU2.MpdConfigError(
                    'MPD dataset error %s: %s' % (rec.errorCode, rec.errorMessage))

        def _native_wait(self, predicate, timeout, message, cancel=False,
                         check_errors=True):
            deadline = time.monotonic() + timeout
            while True:
                self._pump()
                if cancel and self.stop_evt.is_set():
                    raise MCU2.MpdConfigError('STOP received during native recording startup')
                if check_errors:
                    self._native_error()
                if predicate():
                    return
                if time.monotonic() >= deadline:
                    raise MCU2.MpdConfigError(message)
                time.sleep(0.02)

        def _prepare_native(self, session):
            import os
            if not session.online:
                raise MCU2.MpdConfigError('MPD session is not online')
            self.native_rec = rec = session.recording
            if not rec.valid:
                raise MCU2.MpdConfigError('MPD recording controls are not valid')
            # Do not change recording settings or hardware during another recording.
            if rec.recording or self._optional_property(rec, 'recordingBeingStopped', False):
                raise MCU2.MpdConfigError('MPD Suite is already recording or closing a dataset')
            self.native_v2 = self._optional_property(rec, 'recordingBeingStopped') is not None
            today = date.today()
            folder = os.path.join(self.cfg['rec_folder'],
                                  today.strftime('%Y'), today.strftime('%Y%m%d'))
            os.makedirs(folder, exist_ok=True)
            rec.setTargetFolder(folder)
            rec.setBaseFileName('%s_shot_%d' % (self.tree_name, self.shot))
            rec.setDescription('%s shot %d' % (self.tree_name, self.shot))
            if self.native_v2:
                rec.setEnableAutoTermination(False)
                self._native_wait(
                    lambda: rec.targetFolderWritable and rec.recordingAllowed
                            and not rec.enableAutoTermination,
                    self.cfg['init_tmo'],
                    'Native recording is unavailable: check folder permissions and MPD state',
                    cancel=True, check_errors=False)
            else:
                self.log('RecordingControls2 unavailable: using basic dataset controls')
            self.log('Native dataset destination: %s; prefix: %s_shot_%d'
                     % (folder, self.tree_name, self.shot))

        def _start_native(self, session):
            rec = self.native_rec
            if not (session.valid and session.online and rec.valid):
                raise MCU2.MpdConfigError('Online recording session is no longer valid')
            if rec.recording or self._optional_property(rec, 'recordingBeingStopped', False):
                raise MCU2.MpdConfigError('MPD Suite is already recording or closing a dataset')
            if self.native_v2:
                if not (rec.targetFolderWritable and rec.recordingAllowed):
                    raise MCU2.MpdConfigError('MPD recording is not allowed or folder is not writable')
                rec.setEnableAutoTermination(False)
            # A partially successful COM call must still be cleaned up.
            self.native_rec_owned = True
            rec.startRecording()
            self._native_wait(lambda: bool(rec.recording), self.NATIVE_START_TIMEOUT,
                              'Timeout starting native MPD dataset', cancel=True)
            self.native_filename = str(rec.fileName)
            self.log('Native dataset recording started: %s' % self.native_filename)

        def _check_native(self, session):
            if not (session.valid and session.online and self.native_rec.valid):
                raise MCU2.MpdConfigError('MPD online recording session was lost')
            self._native_error()
            if not self.native_rec.recording:
                raise MCU2.MpdConfigError('Native MPD dataset recording stopped unexpectedly')

        def _stop_native(self):
            if not self.native_rec_owned or self.native_rec is None:
                return
            rec = self.native_rec
            rec.stopRecording()
            # Even after a write error, allow finalization to complete before
            # reporting that error. RecordingControls2 explicitly exposes closing.
            self._native_wait(
                lambda: not rec.recording and not self._optional_property(
                    rec, 'recordingBeingStopped', False),
                self.NATIVE_STOP_TIMEOUT, 'Timeout finalizing native MPD dataset',
                check_errors=False)
            self.native_rec_owned = False
            self._native_error()
            self.log('Native dataset closed: %s' % self.native_filename)

        def _cleanup_error(self, context, exc):
            message = '%s: %s' % (context, exc)
            self.log(message)
            self.error = (self.error + '\n' + message) if self.error else message

        # ---------------- ciclo principale ----------------

        def run(self):
            import pythoncom
            M = MCU2
            pythoncom.CoInitialize()
            tree = None
            try:
                tree = Tree(self.tree_name, self.shot)
                mcu = tree.getNode(self.mcu_path)

                # ---- INIT: connessione, ricerca canali, configurazione ----
                app, session = self._connect()
                self._prepare_native(session)

                inv = self._inventory(session)
                inv_text = '\n'.join(M._describe(u) + (' [altra MCU]' if u['other_mcu'] else '')
                                     for u in inv)
                self.log('canali collegati: %d\n%s' % (len(inv), inv_text))
                try:
                    mcu.getNode(':INVENTORY').record = inv_text or 'nessun canale'
                except Exception as exc:
                    self.log('INVENTORY non scritto: %s' % exc)
                if not inv:
                    raise M.MpdConfigError('la MPD Suite non vede nessun canale collegato')

                checks = []
                for ch, unit in self._match(inv):
                    head = mcu.getNode(ch['node'])
                    name = ch['name']
                    typ, typed = self._resolve_type(ch, unit)
                    if typ is None:
                        continue
                    self.log('%s -> %s' % (name, M._describe(unit)))
                    try:
                        head.getNode(':DETECTED').record = M._describe(unit)
                    except Exception as exc:
                        self.log('%s: DETECTED non scritto: %s' % (name, exc))
                    checks += self._apply(head, M.UNIT_DRIVERS[typ], unit['com'], name, typed)

                    rt = M._UnitRuntime(head, unit['com'], None, self.log)
                    rt.path = name
                    self.units.append(rt)

                if not self.units:
                    raise M.MpdConfigError('nessun CH_nn configurato corrisponde a un canale '
                                           'collegato. Canali visti: %s' % inv_text.replace('\n', '; '))

                # ---- controllo richiesto / riletto (solo avvisi) ----
                self._verify(checks)
                if self.warnings:
                    self.log('ARMED con %d differenze di configurazione (vedi sopra)'
                             % len(self.warnings))
                self.log('ARMED: %d canali configurati, in attesa di START'
                         % len(self.units))
                self.ready.set()

                # ---- attesa dello START (o di uno STOP anticipato) ----
                while not (self.start_evt.is_set() or self.stop_evt.is_set()):
                    pythoncom.PumpWaitingMessages()
                    time.sleep(0.01)
                if self.stop_evt.is_set():
                    self.log('STOP ricevuto prima dello START: nessuna acquisizione')
                    return

                # ---- START: apertura snapshot e avvio registrazione ----
                if not session.valid:
                    raise M.MpdConfigError('sessione MPD Suite non piu\' valida')
                self._start_native(session)
                self.recording = True       # da qui lo STOP deve chiudere gli snapshot
                for rt in self.units:
                    rt.snapshot = self._open_snapshot(rt.comch.scope, rt)
                    rt.last_flush = time.monotonic()
                mcu.getNode(':START_UTC').record = M._utc_now()
                self.log('acquisizione avviata su %d canali (%s)'
                         % (len(self.units), self.cfg['acq_mode']))
                self._check_native(session)
                self.started.set()

                poll = self.cfg['acq_mode'] == 'POLL'
                rows, flush_s = self.cfg['seg_rows'], self.cfg['flush_s']
                next_native_check = 0.0
                while not self.stop_evt.is_set():
                    pythoncom.PumpWaitingMessages()
                    now = time.monotonic()
                    if now >= next_native_check:
                        self._check_native(session)
                        next_native_check = now + 0.1
                    for rt in self.units:
                        if poll:
                            rt.poll()
                        if rt.due(rows, flush_s):
                            rt.flush()
                    time.sleep(0.0005)

            except Exception as exc:
                self.error = '%s\n%s' % (exc, traceback.format_exc())
                self.log('ERRORE: %s' % exc)
                # sblocca chi aspetta SOLO dopo aver registrato l'errore
                self.ready.set()
                self.started.set()
            finally:
                try:
                    self._shutdown(tree)
                except Exception as exc:
                    self._cleanup_error('Unexpected shutdown failure', exc)
                finally:
                    try:
                        if self.error and tree is not None:
                            tree.getNode(self.mcu_path).getNode(':STATUS').record = (
                                'ERRORE: ' + self.error)[:3990]
                    except Exception:
                        pass
                    self.ready.set()
                    self.started.set()
                    pythoncom.CoUninitialize()

        def _shutdown(self, tree):
            # Disable callbacks before pumping messages during dataset closing.
            for rt in self.units:
                try:
                    if rt.snapshot is not None:
                        rt.snapshot.setEnabled(False)
                except Exception as exc:
                    self._cleanup_error('Disabling snapshot', exc)
            try:
                self._stop_native()
            except Exception as exc:
                self._cleanup_error('Stopping native dataset', exc)
            if not self.recording:
                return
            for rt in self.units:
                try:
                    rt.flush()
                    rt.write_stats()
                    self.log('%s: %d burst' % (rt.path, rt.n_bursts))
                except Exception as exc:
                    self.log('%s: errore in chiusura: %s' % (rt.path, exc))
                    if self.error is None:
                        self.error = str(exc)
            if tree is not None:
                try:
                    tree.getNode(self.mcu_path).getNode(':STOP_UTC').record = MCU2._utc_now()
                except Exception:
                    pass

    # ------------------------------------------------------------------
    # Metodi del device
    # ------------------------------------------------------------------

    def _part(self, name):
        """Nodo figlio del device. NON usare self.status, self.units ecc.:
        TreeNode ha gia' proprieta' con nomi simili che nascondono le parti."""
        return self.getNode(':' + name)

    def _key(self):
        return '%s:%s:%s' % (MCU2._tree_name(self.tree), self.tree.shot, self.fullpath)

    def _status(self, text):
        if len(text) > 4000:
            text = text[:3990] + ' [...]'
        try:
            self._part('STATUS').record = text
        except Exception:
            pass

    @staticmethod
    def _warn_suffix(worker):
        """Coda di STATUS con le differenze di configurazione trovate all'INIT."""
        if not worker.warnings:
            return ''
        return ' | %d DIFFERENZE DI CONFIGURAZIONE: %s' % (
            len(worker.warnings), '; '.join(worker.warnings))

    def _active_channels(self):
        """Legge e valida la configurazione dei canali (nel thread chiamante,
        cosi' gli errori di configurazione emergono subito)."""
        M = MCU2
        out, seen = [], {}
        for i in range(1, M.N_CH + 1):
            node = '.CH_%02d' % i
            ch = self.getNode(node)
            typ = (M._text(ch.getNode(':TYPE'), '') or '').upper()
            if not typ:
                continue
            label = M._text(ch.getNode(':LABEL'), '')
            name = 'CH_%02d' % i + (' [%s]' % label if label else '')
            if not M._num(ch.getNode(':ENABLED'), 1):
                print('[MPD] %s disabilitato, salto' % name)
                continue
            if typ not in M.UNIT_DRIVERS and typ != 'AUTO':
                M._raise(M.DevBAD_PARAMETER, '%s: TYPE "%s" non valido (validi: %s, AUTO)'
                         % (name, typ, ', '.join(M.UNIT_DRIVERS)))
            # SERIAL vuoto = assegnazione automatica nel thread (vedi _match)
            serial = M._text(ch.getNode(':SERIAL'), '')
            index = int(M._num(ch.getNode(':CHANNEL'), 1))
            if serial:
                if (serial, index) in seen:
                    M._raise(M.DevINV_SETUP, '%s: stessa unita\' (%s, canale %d) di %s'
                             % (name, serial, index, seen[(serial, index)]))
                seen[(serial, index)] = name
            out.append({'node': node, 'name': name, 'type': typ,
                        'serial': serial, 'index': index})
        return out

    def init(self):
        M = MCU2
        key = self._key()
        with M._WORKERS_LOCK:
            old = M._WORKERS.get(key)
            if old is not None and old.is_alive():
                M._raise(M.DevINV_SETUP, 'acquisizione gia\' attiva per %s: esegui STOP' % key)

        channels = self._active_channels()
        if not channels:
            M._raise(M.DevINV_SETUP,
                     'nessun canale attivo: imposta TYPE (%s o AUTO) in almeno un CH_nn'
                     % ', '.join(M.UNIT_DRIVERS))
        acq_mode = (M._text(self._part('ACQ_MODE'), 'POLL') or 'POLL').upper()
        if acq_mode not in ('POLL', 'EVENT'):
            M._raise(M.DevBAD_PARAMETER, 'ACQ_MODE deve essere POLL o EVENT, non %s' % acq_mode)

        rec_folder = M._text(self._part('REC_FOLDER'), '')
        if not rec_folder:
            M._raise(M.DevBAD_PARAMETER, 'REC_FOLDER must be configured')

        cfg = {
            'channels': channels,
            'acq_mode': acq_mode,
            'rec_folder': rec_folder,
            'seg_rows': max(1, int(M._num(self._part('SEG_ROWS'), 200))),
            'flush_s': max(0.05, M._num(self._part('FLUSH_S'), 1.0)),
            'init_tmo': max(1.0, M._num(self._part('INIT_TMO'), 30.0)),
            'mcu_serial': M._text(self._part('SERIAL'), ''),
        }
        worker = M._Mcu2Worker(M._tree_name(self.tree), self.tree.shot, str(self.fullpath), cfg)
        worker.start()
        # timeout: connessione + ~2 s per canale + controllo della rilettura
        if not worker.ready.wait(2.0 * cfg['init_tmo'] + 2.0 * len(channels)
                                 + M.SETTLE_MAX_S + 5.0):
            worker.stop_evt.set()
            self._status('TIMEOUT INIT')
            M._raise(M.DevCOMM_ERROR, 'timeout durante INIT (MPD Suite raggiungibile?)')
        if worker.error:
            worker.join(10)
            self._status('ERRORE INIT: ' + worker.error)
            M._raise(M.DevINV_SETUP, 'INIT fallito: ' + worker.error.splitlines()[0])

        with M._WORKERS_LOCK:
            M._WORKERS[key] = worker
        self._status('ARMED dal %s su %s%s'
                     % (M._utc_now(), ', '.join(rt.path for rt in worker.units),
                        self._warn_suffix(worker)))
        return 1

    def start(self):
        M = MCU2
        key = self._key()
        with M._WORKERS_LOCK:
            worker = M._WORKERS.get(key)
        if worker is None or not worker.is_alive():
            M._raise(M.DevINV_SETUP, 'nessun INIT attivo per %s: esegui prima INIT' % key)
        if worker.start_evt.is_set():
            M._raise(M.DevINV_SETUP, 'acquisizione gia\' avviata per %s' % key)
        worker.start_evt.set()
        n = len(worker.units)
        if not worker.started.wait(worker.NATIVE_START_TIMEOUT + 10.0 + 1.0 * n):
            worker.stop_evt.set()
            self._status('TIMEOUT START; cleanup requested')
            M._raise(M.DevCOMM_ERROR, 'timeout durante START')
        if worker.error:
            # Keep the worker available to STOP, including pending cleanup.
            worker.join(10)
            self._status('ERRORE START: ' + worker.error)
            M._raise(M.DevCOMM_ERROR, 'START fallito: ' + worker.error.splitlines()[0])
        self._status('RUNNING dal %s su %s%s'
                     % (M._utc_now(), ', '.join(rt.path for rt in worker.units),
                        self._warn_suffix(worker)))
        return 1

    def stop(self):
        M = MCU2
        key = self._key()
        with M._WORKERS_LOCK:
            worker = M._WORKERS.get(key)
        if worker is None:
            M._raise(M.DevINV_SETUP, 'nessun INIT/acquisizione attivo per %s' % key)
        worker.stop_evt.set()
        worker.join(60)
        if worker.is_alive():
            self._status('STOP: il thread non si e\' fermato')
            M._raise(M.DevCOMM_ERROR, 'il thread di acquisizione non si e\' fermato entro 60 s')
        with M._WORKERS_LOCK:
            if M._WORKERS.get(key) is worker:
                M._WORKERS.pop(key, None)
        if not worker.recording and not worker.error:
            self._status('STOPPED %s | START non eseguito, nessun dato%s'
                         % (M._utc_now(), self._warn_suffix(worker)))
            return 1
        summary = '; '.join('%s=%d' % (rt.path, rt.n_bursts) for rt in worker.units)
        if worker.error:
            self._status('ERRORE: %s\n%s' % (worker.error, summary))
            M._raise(M.DevCOMM_ERROR,
                     'errore durante l\'acquisizione: ' + worker.error.splitlines()[0])
        self._status('STOPPED %s | %s%s' % (M._utc_now(), summary, self._warn_suffix(worker)))
        return 1