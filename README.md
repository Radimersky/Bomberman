<h1>Domácí úkol č. 3: Bomberman</h1>

<p><a href="https://classicreload.com/bomberman.html">Bomberman</a> je známá počítačová hra, jejíž první verze vyšla již v roce 1983. Nevíme, jak dlouho tehdy trval její vývoj, ale my ji napíšeme za pár hodin – v této domácí úloze. K dispozici budeme mít moderní programovací jazyk (C++), grafickou knihovnu (SDL2) a předpřipravenou kostru programu. Výsledek bude vypadat nějak takto:</p>
<p><img src="game_with_fire.png" /></p>
<p>(Vzorová implementace je spustitelná na strojích <code>nymfeXY</code> (linuxové stroje v PC hale a B130) v <code>/home/xtusil/bomberman</code>.)</p>
<p>Hru mohou hrát dva hráči; jeden hráč k ovládání využívá klávesy asdw a mezerník, druhý šipky a enter. Po stáhnutí šablony, její kompilaci a spuštění se zobrazí okno s herní mapou a postavičkou.</p>
<p><img src="template.png" /></p>
<p>Cílem této úlohy je dokončit vše ostatní, naučit se přitom zásadám objektového návrhu a vyzkoušet si práci na větším projektu s již existujícím kódem.</p>
<p>Autor doporučuje dívat se během čtení následující sekce do zdrojových kódů projektu.</p>
<h1 id="architektura-hry">Architektura hry</h1>
<p>Z hlediska kompilace a <code>CMakeLists.txt</code> je hra rozdělená na tři komponenty: knihovnu <code>bomber</code>, obsahující hlavní herní logiku; spustitelný program pro testy (<code>testbomb</code>), který knihovnu <code>bomber</code> testuje; a spustitelný program <code>bomberman</code>, který kromě knihovny <code>bomber</code> používá i grafickou knihovnu SDL a implementuje vykreslování stavu hry a interakci s uživatelem. Úlohu je tak možné napsat i otestovat i bez použití knihovny SDL.</p>
<p>Hra (program <code>bomberman</code>) běží v cyklu, který je ukončený zavřením okna (například klávesou Escape). V tomto cyklu, jež je implementován v souboru <code>main.cpp</code> ve funkci <code>App::main</code>, periodicky dochází k aktualizaci stavu hry (<code>App::update</code>), zpracování událostí z knihovny SDL (<code>App::handle_sdl_events</code>) a vykreslení obrazovky (<code>App::render</code>). Vykreslení spočívá ve vykreslení mřížky (která znázorňuje mapu – <code>Grid::drawGrid</code>) a všech herních objektů (hráči, bomby, stěny, … – <code>App::renderMapItems</code>). O vykreslování se více zajímat nemusíte.</p>
<p>Aktualizace stavu hry (<code>App::update</code>) se skládá ze dvou částí. Nejprve dojde k předání příkazů uživatelsky řízeným postavám (<code>App::playerControl</code>), kdy se nasbírané události (typu “stisk mezerníku”) interpretují jako akce s příslušnou instancí třídy <code>Player</code> (volání metody <code>Player::bomb()</code>). Toto je implementováno ve třídě <code>PlayerControl</code>. Poté dojde k takzvanému <em>hernímu ticku</em>.</p>
<p>Koncept <a href="https://gamedev.stackexchange.com/questions/81608/what-is-a-tick-in-the-context-of-game-development">herního ticku</a> je ve světě počítačových her <a href="https://www.reddit.com/r/explainlikeimfive/comments/4mn531/eli5_what_is_atick_in_gaming_development/">hojně</a> <a href="https://minecraft.gamepedia.com/Tick">rozšířený</a>. V naší hře se herní tick (<code>Playground::gameTick</code>) skládá ze tří fází:</p>
<ol type="1">
<li>provedení akcí herních objektů (<code>IGameObject::gameTick()</code>).</li>
<li>odstranění starých objektů (například vybuchlých bomb) z hracího pole (<code>Playground::removeExpired()</code>); a</li>
<li>přidání nově vytvořených objektů do hracího pole (<code>Playground::addNewObjects()</code>).</li>
</ol>
<p>A právě v akcích vykonávaných herními objekty v první fázi herního ticku bude implementována hlavní herní logika: pohyby hráčů, sbírání bonusů i vybuchování bomb.</p>
<h2 id="herní-pole-políčka-podpolíčka-pozice">Herní pole, políčka, podpolíčka, pozice</h2>
<p>Celý děj hry se odehrává na herním poli, jež je reprezentováno třídou <code>Playground</code>. Pole je rozdělené mřížkou na čtvercová políčka, jejichž počet a iniciální obsah závisí na použité mapě (viz soubor <code>MapParser.h</code>).</p>
<p>V herním poli se nacházejí herní objekty. Tyto jsou většinou zarovnané na políčka herního pole, ale nutně tomu tak být nemusí – zejména hráč se může a má pohybovat s jemnějším rozlišením. Z tohoto důvodu je každé herní políčko rozdělené na <code>n</code> stejně velkých částí v horizontálním i vertikálním směru, a tedy se skládá z <code>n</code>×<code>n</code> podpolíček. Hodnota <code>n</code> je dána konstantou <code>Config::mesh</code>, což je obvykle <code>7</code>. Herní objekty se tedy pohybují v takto definované jemnější mřížce.</p>
<p>Herní objekty se nachází na pozicích reprezentovaných třídou <code>Position</code>. Pozice má vždy čtvercový tvar a skládá se z podpolíček jemnější mřížky. Každá pozice je jednoznačně určena svou velikostí (délkou strany čtverce) a souřadnicemi podpolíčka ve levém horním rohu čtverce – viz <code>Geometry.h</code>.</p>
<p>O vykreslování vlastní mřížky i kreslení do jednotlivých políček se stará třída <code>Grid</code>. Její metoda <code>Grid::fill</code> vezme pozici (<code>Position</code>) a vyplní ji barvou; metoda <code>Grid::draw</code> vezme pozici a vykreslí do ni texturu. Tím je vyřešeno vykreslování herních objektů, protože s každým objektem je spojena pozice i textura.</p>
<p>Pozice herních objektů mají obvykle velikost políčka herního pole, avšak herní logika je na tomto faktu nezávislá.</p>
<h2 id="herní-objekty">Herní objekty</h2>
<p>Herními objekty nazýváme všechny objekty nacházející se na mapě. Mezi ně patří: hráč (třída <code>Player</code>); bomba (<code>Bomb</code>); cihlová či kamenná stěna (<code>Wall</code>); plamen (<code>Flame</code>) – vznikne při výbuchu bomby; a bonusový předmět (<code>BonusItem</code>), který někdy vypadne ze zničené stěny. Tyto objekty se všechny chovají podobně v tom smyslu, že mají svou pozici na mapě a v každém herním ticku mohou udělat nějakou akci (pohnout se, vybuchnout). Proto všechny implementují rozhraní <code>IGameObject</code>.</p>
<h3 id="igameobject">IGameObject</h3>
<p>Rozhraní <code>IGameObject</code> umožňuje vnějšímu kódu provádět s herními objekty následující činnosti:</p>
<ul>
<li>Zjišťovat a měnit pozice objektů (pomocí metod s názvem <code>position</code>).</li>
<li>Pracovat s texturou objektu (pomocí metod s názvem <code>texture</code>).</li>
<li>„Navštívit“ herní objekt a vykonat akci v závislosti na typu objektu (pomocí metod s názvem <code>visit</code>). Této technice se říká <em>Visitor Pattern</em> a bude popsána dále.</li>
</ul>
<p>Toto rozhraní však také („na oplátku“) něco umožňuje objektu. Vnější kód (<code>Playground::gameTick</code>) pravidelně (v každém ticku) volá metodu <code>gameTick</code> každého objektu žijícího na mapě. Objekt pak může reagovat na měnící se stav hry téměř libovolným způsobem.</p>
<h3 id="gameobjectbase">GameObjectBase</h3>
<p>Když se ve vícero třídách implementujících stejné rozhraní objevuje podobný nebo totožný kód, je vhodné (v souladu s principem <a href="https://en.wikipedia.org/wiki/Don%27t_repeat_yourself">DRY</a>) tento kód vyextrahovat do samostatné třídy, které se někdy říká <em>Base class</em> nebo <em>Skeleton class</em>. Z této třídy pak ostatní dědí, čímž se vyhneme duplicitě kódu. Herní objekty jsou jedním z takovýchto případů: společnou funkcionalitou je zde například práce s referencemi textur.</p>
<p>Někdo by mohl chtít implementovat tuto funkcionalitu přímo ve třídě <code>IGameObject</code>, protože z ní dědí všechny herní objekty. V praxi je však vhodné <a href="https://softwareengineering.stackexchange.com/a/142193/273974">oddělovat</a> rozhraní od implementace a programovat jen <a href="https://stackoverflow.com/q/383947/6209703">vzhledem k rozhraní</a>, nikoliv vzhledem k implementaci. Proto máme v projektu třídu <code>GameObjectBase</code>, která je takovýmto skeletonem pro herní objekty. Na to, která funkcionalita do ní přijde, však musíte přijít sami ;).</p>
<h3 id="vytváření-herních-objektů">Vytváření herních objektů</h3>
<p>Naše herní objekty implementují komplexní herní logiku. Například třída <code>Player</code> musí umět vytvářet instance třídy <code>Bomb</code> a začleňovat je do běhu hry; naopak <code>Bomb</code> musí být schopná kromě generování plamenů i ničit jiné objekty. Protože herní objekty potřebují mít přístup k části stavu hry, během vytváření instance herního objektu je třeba předat objektu ukazatele/reference na všechno, co může potřebovat. Kromě toho je také třeba nastavit objektu texturu. Bylo by ale nepraktické, kdyby si například hráč musel držet referenci na texturu plamenů jen proto, že může položit bombu, která může vybuchnout a vytvořit plameny. Proto je za vytváření objektů zodpovědná třída <code>GameObjectFactory</code>. Ta obsahuje metody typu</p>
<pre><code>std::unique_ptr&lt;Wall&gt; createBrick() const;</code></pre>
<p>jež volajícímu vrátí nový herní objekt požadovaného druhu. Všimněme si tu dvou věcí:</p>
<ul>
<li>Metody vracejí objekt typu <code>unique_ptr</code> a nově vytvářený objekt tedy předávají do správy volajícího.</li>
<li>Metody jsou označené kvalifikátorem <code>const</code> a nemohou tedy měnit stav <code>GameObjectFactory</code>.</li>
</ul>
<h3 id="životní-cyklus-herních-objektů">Životní cyklus herních objektů</h3>
<p>O správu vytvořených objektů se stará třída <code>Playground</code>. Obsahuje také nějaké metody pro vytváření objektů, ty ale vypadají jinak než v případě <code>GameObjectFactory</code>:</p>
<pre><code>Wall &amp; createBrick();</code></pre>
<p>Tyto metody vytvoří novou instanci herního objektu, nechají si ji ve správě a vrátí na ni referenci. Volající nový objekt nevlastní, jen jej dostane k používání.</p>
<p>Při zavolání metody typu <code>Playground::createXyz</code> si třída Playground nově vzniklý objekt zařadí do seznamu <em>nových objektů</em>. Nové objekty jsou ty, které sice již byly fyzicky vytvořeny, ale ještě nejsou součástí “pravidel hry”. To znamená, že metody <code>Playground::forEach</code> ani <code>Playground::visitAll</code> je nevidí, a ani metody <code>IGameObject::gameTick</code> těchto objektů nejsou ve fázi 1 herního ticku volány. Teprve během nejbližší fáze 3 herního ticku jsou všechny nové objekty přemístěny do <em>seznamu aktivních objektů</em>. Během fáze 1 následujícího herního ticku jsou již tyto objekty viditelné v <code>Playground::forEach</code> i <code>Playground::visitAll</code>, a i jejich <code>IGameObject::gameTick</code> funkce budou volány.</p>
<p>O herním objektu říkáme, že je <em>aktivní v ticku t</em>, pokud se v první fázi ticku t nachází v <em>seznamu aktivních objektů</em>. Aktivním herním objektům také říkáme <em>objekty na hrací ploše</em> nebo <em>objekty ve hře</em>.</p>
<p>Třída <code>Playground</code> také obsahuje funkci <code>void remove(IGameObject &amp; object)</code> pro odebírání objektů ze hry. Objekty předané této funkci si <code>Playground</code> označí jako <em>objekty k odebrání</em>. Tyto objekty budou zničeny v nejbližší fázi 2 herního cyklu; do té doby jsou však stále vedeny jako aktivní.</p>
<p>Metody <code>Playground::createXyz</code> a <code>Playground::remove</code> jsou navrženy pro to, aby bylo volány ve fázi 1 herního cyklu. Pokud tomu tak je, pak pojmy “nejbližší fáze 2 (nebo 3) herního cyklu” znamenají totéž, co “fáze 2 (nebo 3) současného herního cyklu”. Protože k volání destruktorů herních objektů dochází ve fázi 2, není rozumné v těchto destruktorech volat metody třídy <code>Playground</code>.</p>
<p>Metody <code>gameTick</code> herních objektů jsou volány pouze ve fázi 1 herního ticku a nikdy jindy.</p>
<h2 id="visitory">Visitory</h2>
<p>Rozhraní <code>IGameObject</code> také poskytuje metody</p>
<pre><code>virtual void visit(IObjectVisitor &amp; visitor) = 0;
virtual void visit(IConstObjectVisitor &amp; visitor) const = 0;</code></pre>
<p>za účelem použití techniky zvané <a href="https://en.wikipedia.org/wiki/Visitor_pattern">Visitor Pattern</a>. Když chceme s herním objektem provést akci, která závisí na typu herního objektu, implementujeme novou třídu – nový visitor. Tato třída pro každý typ herního objektu obsahuje samostatnou funkci s akcí pro příslušný typ herního objektu.</p>
<p>Naše visitory implementují rozhraní <code>IObjectVisitor</code> či <code>IConstObjectVisitor</code> v závislosti na tom, zda navštěvovaný objekt potřebují modifikovat či nikoli. V souboru <code>ObjectVisitorSkeleton.h</code> se pak nachází kostry visitorů, které je možné použít jako rodičovské třídy těch visitorů, které nepotřebují implementovat všechny metody. Ukázkový visitor <code>DeadPlayerDetector</code> je umístěný v souboru <code>Visitors.h</code>; do tohoto souboru můžete umístit i některé své visitory, které chcete mít dostupné z vícero míst.</p>
<p>Podrobnější popis této techniky je v <a href="#přílohy">příloze</a>.</p>
<h1 id="zadání">Zadání</h1>
<p>Velká část hry včetně grafiky je již implementovaná; vaší úlohou je jen doplnit implementaci herních objektů a jejich továrny. Veřejné a neveřejné metody můžete doplňovat dle libosti, jen přitom dodržujte principy dobrého návrhu. Soubory, které neodevzdáváte, můžete modifikovat, jak se vám zachce, ale Kontr bude pracovat s původními soubory.</p>
<h2 id="třída-gameobjectfactory">Třída <code>GameObjectFactory</code></h2>
<p>Implementujte všechny metody třídy <code>GameObjectFactory</code>. Nezapomeňte herním objektům nastavit správné textury; šablona už obsahuje názvy textur, které je třeba použít. Objekty vytvářené metodami továrny musí být plně použitelné.</p>
<h2 id="herní-objekty-1">Herní objekty</h2>
<p>Implementujte herní objekty <code>Wall</code>, <code>BonusItem</code>, <code>Flame</code>, <code>Player</code>, <code>Bomb</code>.</p>
<ol type="1">
<li>Všechny objekty musí implementovat rozhraní IGameObject. To je základní <a href="https://en.wikipedia.org/wiki/Protocol_(object-oriented_programming)">interface</a> pro herní objekty. Na začátku souboru <code>IGameObject.h</code> si můžete všimnout použití <a href="https://en.wikipedia.org/wiki/Forward_declaration">dopředné deklarace</a>.</li>
<li>Je potřeba implementovat metody <code>visit</code> – kvůli <a href="https://en.wikipedia.org/wiki/Visitor_pattern">VisitorPatternu</a>. To bude snadné, protože s předaným visitorem doopravdy můžete udělat jen jednu věc.</li>
<li>Konstruktory herních objektů jsou čistě ve vaší režii. My budeme v testech vytvářet herní objekty jen přes <code>GameObjectFactory</code>.</li>
</ol>
<h3 id="třída-player">Třída <code>Player</code></h3>
<p>Tato třída reprezentuje hráče. Hráč se umí pohybovat, pokládat bomby a sbírat bonusy.</p>
<h4 id="pohyb-hráče">Pohyb hráče</h4>
<p>Po zavolání metody <code>move</code> se hráč začne rychlostí jedno podpolíčko za herní tick pohybovat ve zvoleném směru, a tento pohyb trvá po určenou dobu. Přesněji:</p>
<ol type="1">
<li>Hráč nově vytvořený pomocí <code>GameObjectFactory</code> se nepohybuje. Jeho <code>IGameObject::position()</code> se zavoláním jeho <code>gameTick</code> nezmění.</li>
<li>Třída hráče musí definovat metodu <code>void move(Direction direction, int ticks);</code>. Může přitom předpokládat, že parametr <code>tick</code> této metody bude vždy kladné nenulové číslo. Také může předpokládat, že v okamžiku volání <code>move</code> bude hráč na hrací ploše. Tato metoda bude vždy volána mimo herní tick, a to jak testy, tak grafickou hrou.</li>
<li>Hráč může být v procesu pohybu. Hráč je v procesu pohybu ve směru <code>direction</code> od dokončení volání metody <code>move</code> až po dokončení volání <code>ticks</code>-tého volání jeho metody <code>gameTick</code>.</li>
<li>Je-li hráč v procesu pohybu, při zavolání jeho metody <code>gameTick</code> se pohne o jedno podpolíčko ve směru svého pohybu.</li>
<li>Co se stane, když se na pohybujícím se hráči zavolá <code>setPosition</code> nebo <code>move</code>, není specifikované.</li>
<li>Metoda <code>bool isMoving() const;</code> vrátí <code>true</code> právě tehdy, když je hráč v procesu pohybu; v opačném případě vrátí <code>false</code>.</li>
<li>Hráč nemůže chodit přes stěny ani přes bomby. Přesněji: pokud by se měl hráč pohnout na pozici, která se překrývá (ve smyslu funkce <code>overlap</code> z <code>Geometry.h</code>) s pozicí stěny nebo bomby (výjimka viz následující bod). Přes všechny ostatní objekty chodit může.</li>
<li>Pokud hráč položí bombu, tato mu nemůže bránit v pohybu na sousední volné políčko. Přesněji: jím položená bomba brání hráči v pohybu až od herního ticku, ve kterém se hráč s touto bombou nepřekrývá. Nachází-li se hráč mimo hrací plochu, není specifikované, co se stane, začne-li se pohybovat.</li>
<li>Hráč nesmí odejít z hrací plochy.</li>
<li>Při volání <code>gameTick</code> hráč provede pohyb dříve, než dojde ke sbírání bonusů a vyhodnocování kolizí s plameny.</li>
</ol>
<h3 id="vyhýbání-se-překážkám">Vyhýbání se překážkám</h3>
<p>Hráč je tedy blokovaný překážkami. To naneštěstí poněkud ztěžuje hratelnost ve chvíli, kdy se hráč potřebuje strefit do mezery mezi dvěma překážkami. Hráč na následujícím obrázku například nemůže jít doprava, protože mu v pohybu brání horní cihlová stěna. Aby doprava mohl jít, musí nejprve popojít kousek směrem dolů. Pokud se ale dostane příliš dolů, bude mu v pohybu vpravo bránit dolní cihlová stěna. S rostoucí jemností (<code>Config::mesh</code>) se tak pohyb hráče stává složitějším.</p>
<p><img src="blocked-hard-to-play.png" /></p>
<p>Klasická implementace <a href="https://classicreload.com/bomberman.html">DynaBlaster</a> řeší problém tak, že pokud hráči v pohybu brání překážka, tak ji hráč obejde. Naše implementace bude fungovat stejně. Následuje popis algoritmu, který za tímto účelem použijeme.</p>
<p>Hráč, jehož pozice je zarovnaná na herní políčko (tzn. <code>isCentered(player.position()) == true</code>), může být ve svém pohybu ve zvoleném směru blokován jedině překážkou na jednom políčku – překážkou přímo před ním ve směru pohybu. V takovém případě překážku obcházet nebudeme.</p>
<p><img src="blocked-directly2.png" /></p>
<p>Pokud hráč není zarovnaný podle osy kolmé na směr pohybu, v jeho pohybu mu může bránit překážka na jednom ze dvou políček. Vzhledem ke směru pohybu se jedná o políčka vpravo vpředu a vlevo vpředu.</p>
<p>V takovémto případě se pokusíme překážku obejít následujícím způsobem. Mohl by se hráč v daném směru pohnout v případě, že by stál zarovnaný na to políčko, na kterém stojí více (tj. s nímž se více překrývá)? Pokud ne, hráč je blokovaný; pokud ano, hráč se nejprve postupně přesune na zarovnanou pozici a až potom vykoná pohyb v požadovaném směru. K zaokrouhlení pozice slouží funkce <code>round</code> z <code>Geometry.h</code>.</p>
<p>Protože se hráč může pohnout jen o jedno podpolíčko za herní tik, může se stát, že pohyb hráče skončí dříve, než dojde na zarovnanou pozici. V takovém případě hráč ujde jen tolik podpolíček, kolik stihne. Uvažme situaci na obrázku níže. Pokud má hráč jít směrem dolů, tak záleží na tom, jestli se nachází více na levém políčku nebo na pravém. Pokud se bude nacházet více na pravém políčku, v prvních několika herních ticích půjde hráč vpravo. Pokud bude hráč v pohybu i poté, po zarovnání se na pravé políčko půjde hráč dolů.</p>
<p><img src="blocked-both-left2.png" /></p>
<p>V případě pochybností ohledně obcházení překážek se řiďte referenční implementací.</p>
<h4 id="pokládání-bomby">Pokládání bomby</h4>
<p>Třída hráče musí obsahovat metodu <code>void bomb()</code>, při jejímž zavolání se hráč pokusí položit bombu na cílové políčko. Cílové políčko je to (celé) políčko, na němž hráč stojí nejvíce. Tato pozice je výsledkem volání funkce <code>round</code> z <code>Geometry.h</code> na pozici hráče. Pokus položit bombu bude úspěšný právě tehdy, když má hráč nějakou bombu v zásobě a když se na cílovém políčko žádná bomba nenachází. Neúspěšný pokus položit bombu nemá žádný efekt.</p>
<p>K vytvoření bomby použijte metodu <code>Playground::createBomb()</code>. Tato funkce bombu vytvoří ihned, bomba však bude do seznamu aktivních herních objektů zařazena až v nejbližší fázi 3 herního ticku (viz životní cyklus).</p>
<p>Metoda <code>Player::bomb</code> je volána mimo herní tick.</p>
<h3 id="třída-wall">Třída <code>Wall</code></h3>
<p>Zdi (instance třídy <code>Wall</code>) máme cihlové, které jsou snadno zničitelné bombou, a kamenné, které jsou nezničitelné. Proto musí třída <code>Wall</code> implementovat metodu</p>
<pre><code>void destroyable(bool value);</code></pre>
<p>pro nastavení zničitelnosti.</p>
<p>Každá stěna zastavuje šíření plamene z výbuchu bomby. Když je zničitelná stěna v dosahu bomby, bude při jejím výbuchu zničena. Za zničitelnou stěnou se navíc může schovávat bonus, který z ní vypadne při jejím zničení. Proto musí třída <code>Wall</code> implementovat metodu</p>
<pre><code>void bonus(Bonus value);</code></pre>
<p>pro nastavení bonusu; hodnota <code>Bonus::None</code> přitom znamená, že zeď žádný bonus neukrývá. Jakým způsobem bude váš kód zjišťovat, zda je daná instance zničitelná a jaký bonus skrývá, to je čistě na vás.</p>
<p>Na vás je též zařídit, aby byla stěna při výbuchu zničena a smazána z mapy (metodou <code>Playground::remove()</code>).</p>
<h3 id="třída-bonusitem">Třída <code>BonusItem</code></h3>
<ol type="1">
<li>Při zničení <code>Wall</code> objektu ukrývajícího bonus dojde k vytvoření <code>BonusItem</code>. Bonusový objekt musí být na hrací ploše právě od prvního herním ticku, ve kterém zničená zeď již není na mapě. Toho lze dosáhnout tím, že se volání funkce <code>Playground::createBonusItem</code> pro vytvoření bonusového objektu a volání funkce <code>Playground::remove</code> pro odstranění zdi uděje ve fázi 1 stejného herního ticku.</li>
<li><code>BonusItem</code> je zničitelný výbuchem bomby a výbuch nezastavuje, nýbrž propouští jej dále.</li>
<li>Zjistí-li hráč během svého <code>gameTick</code>, že se překrývá (<code>overlap</code> z <code>Geometry.h</code>) s <code>BonusItem</code>, vezme jej. V takovém případě je bonusový objekt odebrán z hrací plochy okamžitě (<code>Playground::removeNow</code>). Pokud by tedy došlo k situaci, že by se jeden bonusový objekt na začátku fáze 1 herního ticku překrýval s vícero hráči, přivlastní si jej pouze jeden hráč – ten, jehož <code>gameTick</code> byl zavolán jako první.</li>
</ol>
<p>Významy jednotlivých bonusových položek jsou dané výčtovým typem <code>Bonus</code> v souboru <code>Bonus.h</code>.</p>
<h4 id="bombrange"><code>BombRange</code></h4>
<ol type="1">
<li>S každou bombou je spojena vlastnost, které říkáme <em>síla</em>.</li>
<li>S každým hráčem je spojena vlastnost, které říkáme <em>síla bomb</em>.</li>
<li>Bomba čerstvě vytvořená voláním <code>Playground::createBomb</code> má <em>sílu</em> 1.</li>
<li>Položí-li hráč bombu, <em>síla</em> této bomby je stejná jako <em>síla bomb</em> hráče v tomto okamžiku.</li>
<li>Počáteční <em>síla bomb</em> hráče zkonstruovaného pomocí <code>Playground::createPlayer</code> je 1.</li>
<li>Vezme-li hráč bonusovou položku typu <code>BombRange</code>, <em>síla bomb</em> tohoto hráče se zvýší o 1.</li>
</ol>
<h4 id="bombmax"><code>BombMax</code></h4>
<ol type="1">
<li>S každým hráčem jsou spojeny vlastnosti <em>počet aktivních bomb</em> a <em>maximální počet aktivních bomb</em></li>
<li>Počet aktivních bomb hráče je iniciálně 0; inkrementuje se položením bomby hráčem; dekrementuje se výbuchem hráčem položené bomby.</li>
<li>Maximální počet aktivních bomb hráče je iniciálně 1.</li>
<li>Počet bomb v zásobě hráče je jeho maximální počet aktivních bomb umenšený o jeho počet aktivních bomb.</li>
<li>Vezme-li hráč bonusovou položku typu <del><code>BombRange</code></del> <code>BombMax</code>, maximální počet aktivních bomb tohoto hráče se zvýší o 1.</li>
</ol>
<h3 id="poison"><code>Poison</code></h3>
<p>Vezme-li hráč bonusový objekt typu <code>Poison</code>, hráč bude po dobu danou konstantou <code>Config::poisonTome</code> všechny pohyby vykonávat naopak. Přesněji:</p>
<ol type="1">
<li>vezme-li hráč bonusový objekt ve volání svého <code>gameTick</code> ve fázi 1 <code>t</code>-tého ticku hry, bude ve všech herních ticích v období počínaje tickem <code>t+1</code> a konče tickem <code>t+Config::poisonTime</code> (včetně) otrávený</li>
<li>Je-li hráč v herním ticku otrávený, tak v tomto ticku vykoná pohnutí z bodu 4. ze sekce <a href="#pohyby-hrace">Pohyby hráče</a> v opačném směru oproti běžnému (neotráveném) stavu. Využijte funkci <code>opposite</code> z <code>Geometry.h</code>.</li>
</ol>
<p>Pro přehled poznamenáváme dva přímé důsledky této definice:</p>
<ol type="1">
<li>Otrava se vztahuje i na pohyb započatý (voláním funkce <code>move</code>) již před vzetím bonusu.</li>
<li>V ticku následujícím po otrávení již otráveného hráče se tento hráč chová stejně, jako kdyby k původní otravě nedošlo a on by byl otrávený pouze podruhé.</li>
</ol>
<p>Správně by se měl efekt jedu zpracovat před pokusem o vyhnutí se překážky, tato kombinace však není testována.</p>
<h3 id="třída-flame">Třída <code>Flame</code></h3>
<ol type="1">
<li>Každá instance třídy <code>Flame</code> musí být ve hře právě po dobu <code>Config::fireTime</code> herních ticků. To lze zařídit třeba použitím funkce <code>Playground::remove</code> při <code>Config::fireTime</code>-tém zavoláním metody <code>gameTick</code> na dané instanci <code>Flame</code>.</li>
<li>Pokud se ve fázi 1 herního ticku překrývá plamen a hráč, tento hráč je popálen. <del>(Protože plameny i hráči jsou odebíráni vždy jen ve fázi 2 herního ticku a nikdy ve fázi 1, nezáleží na tom, zda je tato podmínka testována z <code>gameTick</code> hráče nebo plamenu – v obou případech bude vyhodnocena stejně.)</del></li>
<li>Při popálení hráče dojde k zavolání metody <code>Playground::killPlayer</code>. Do tohoto okamžiku volání metody <code>Player::isDead</code> na onom hráči vrací <code>false</code>, poté již vrací <code>true</code>.</li>
<li><del>K popálení hráče dojde i v situaci středu hráče s explozí, neboť při explozi je generován plamen.</del></li>
</ol>
<h3 id="třída-bomb">Třída <code>Bomb</code></h3>
<p>Jak již bylo řečeno při popisu <code>BombRange</code>, každá bomba má určitou <em>sílu</em>, která určuje, jak daleko od ní se šíří exploze. Počáteční <em>síla</em> bomby vytvořené přímo pomocí <code>GameObjectFactory</code> je 1; <em>síla</em> bomby položené hráčem je stejná jako <em>síla bomb</em> hráče v okamžiku jejího položení.</p>
<ol type="1">
<li>Při <code>Config::bombTime</code>-tém volání <code>gameTick</code> bomby dojde k jejímu výbuchu.</li>
<li>Pokud se ve fázi 1 herního ticku překrývá bomba s plamenem, dojde k jejímu výbuchu.</li>
<li>V herním ticku, ve kterém dojde k výbuchu bomby, je tato bomba odstraněna, takže v následujícím herním ticku již není mezi aktivními herními objekty.</li>
<li>Bomba při výbuchu vygeneruje plameny ve tvaru písmene plus (<code>+</code>). Plameny přitom mohou dosáhnout až do vzdálenosti tolika políček od bomby, jaká je síla bomby. Plameny se však také zastaví o zeď (<code>Wall</code>); na místě zdi plamen vygenerován není. Pokud se šíření plamenů zastaví o zničitelnou zeď, je tato zeď zničena. <strong>Pokud se plameny rozšíří na políčko obsahující bonusový objekt, tento je zničen.</strong> Ke zničení zdi <strong>nebo bonusového objektu</strong> dojde <del>v tomtéž herním ticku</del> zavoláním metody <code>Playground::remove</code> <strong>ve fázi 1 toho herního ticku, ve kterém došlo k výbuchu</strong>.</li>
</ol>
<p>Je vhodné, aby bomba při výbuchu zavolala <a href="https://en.wikipedia.org/wiki/Callback_(computer_programming)">callback</a>, přes který se její majitel dozví o jejím výbuchu. Bomba však musí korektně fungovat i v případě, že byla vyrobena jednoduše přes <code>Playground::createBomb</code> a nikdo ji žádný callback nenastavil.</p>
<p>Pro šíření plamenů využijte funkci <code>next</code> z <code>Geometry.h</code>.</p>
<h2 id="soubory-v-projektu">Soubory v projektu</h2>
<p>Odevzdáváte následující soubory</p>
<ul>
<li><code>Bomb.cpp</code>, <code>Bomb.h</code>,</li>
<li><code>BonusItem.cpp</code>, <code>BonusItem.h</code>,</li>
<li><code>Player.cpp</code>, <code>Player.h</code>,</li>
<li><code>Flame.cpp</code>, <code>Flame.h</code>,</li>
<li><code>Wall.cpp</code>, <code>Wall.h</code>,</li>
<li><code>GameObjectFactory.h</code>, <code>GameObjectFactory.cpp</code>,</li>
<li><code>GameObjectBase.h</code>,</li>
<li><code>Visitors.h</code>,</li>
</ul>
<p>Soubory, které by vás měly zajímat:</p>
<ul>
<li><code>IGameObject.h</code>, <code>IObjectVisitor.h</code></li>
<li><code>Bonus.h</code> – Obsahuje seznam všech typů bonusů.</li>
<li><code>ObjectVisitorSkeleton.h</code></li>
<li><code>Playground.h</code> – budete využívat rozhraní třídy <code>Playground</code></li>
<li><code>Config.h</code>, <code>Config.cpp</code> – Různé konstanty.</li>
<li><code>Geometry.h</code> – Např. funkce <code>overlap()</code>. Implementaci zkoumat nemusíte.</li>
<li><code>TextureRef.h</code></li>
<li><code>Dimensions.h</code> – Abyste neutekli z hrací plochy :)</li>
<li><code>Bomberman.h</code></li>
</ul>
<p>Co číst nemusíte:</p>
<ul>
<li><code>Geometry.cpp</code></li>
<li><code>Dimensions.cpp</code></li>
<li><code>Playground.cpp</code></li>
<li><code>Bomberman.cpp</code></li>
<li><code>main.cpp</code></li>
<li><code>Grid.h</code>, <code>Grid.cpp</code> – vykreslování mřížky</li>
<li><code>Textures.h</code>, <code>Textures.cpp</code></li>
<li><code>MapParser.h</code>, <code>MapParser.cpp</code></li>
<li><code>SDLWrapper.h</code>, <code>SDLWrapper.cpp</code></li>
<li><code>PlayerControl.h</code>, <code>PlayerControl.cpp</code></li>
<li><code>Rgb.h</code></li>
</ul>
<h1 id="poznámky">Poznámky</h1>
<h2 id="konvence">Konvence</h2>
<p>Při rozsáhlých projektech, na nichž pracuje vícero lidí, je potřeba dodržet určité konvence. V tomto projektu dodržujeme následující:</p>
<ol type="1">
<li>Jména soukromých členských proměnných tříd končí podtržítkem (<code>mojekocicka_</code>)</li>
<li>Jména metod dodržují <code>camelCase</code> formát.</li>
<li>Metody, které přepisují (<code>override</code>) virtuální metody nějaké rodičovské třídy, jsou v deklaracích tříd seskupeny.</li>
</ol>
<h2 id="doporučený-postup-implementace">Doporučený postup implementace</h2>
<p>Při psaní řešení této domácí úlohy je vhodné kód průběžně testovat, a to jak ruční kontrolou (hraním hry), tak pomocí automatických testů. Využijte přiloženou testovací infrastrukturu.</p>
<p>Aby byla hra hratelná, je třeba smazat definici makra <code>FACTORY_NOT_IMPLEMENTED</code> ze souboru <code>Bomberman.h</code>. Je-li toto makro definované, hra (CMake target <code>bomberman</code>) nepracuje s herními objekty a jen zobrazuje ikonku panáčka uprostřed mapy. Před smazáním tohoto makra je ale třeba, aby <code>GameObjectFactory</code> dokázala vytvořit instanci hráče a všeho, co se nachází na použité mapě.</p>
<p>Doporučený postup implementace, který respektuje uvedená omezení, je následující:</p>
<ol type="1">
<li>Implementovat <code>gameObjectFactory::createPlayer()</code> a otestovat ji testy.</li>
<li>Smazat <code>FACTORY_NOT_IMPLEMENTED</code> z <code>Bomberman.h</code> a použít mapu bez překážek.</li>
<li>Implementovat a otestovat třídu <code>Player</code> tak, aby se hráč mohl pohybovat.</li>
<li>Naučit se vyrábět stěny (<code>gameObjectFactory::createBrick()</code>), použít mapu se stěnami a naučit hráče základní interakci s nimi a s okrajem herní plochy. Otestovat.</li>
<li>Implementovat a otestovat pokládání bomby hráčem a interakci s bombou.</li>
<li>Výbuch bomby a generování plamene.</li>
<li>Interakce plamenů s herními objekty.</li>
<li>Sbírání bonusů hráčem.</li>
<li>Obcházení překážek hráče,.</li>
</ol>
<p>Toto pořadí má tu výhodu, že po každém splněném (a otestovaném) kroku je možné hru spustit a podívat se, jak se zlepšila.</p>
<h2 id="ostatní">Ostatní</h2>
<ul>
<li>Vyhněte se duplikaci kódu.</li>
<li>Používejte <code>const</code> a <code>override</code> všude, kde to má smysl.</li>
<li>Nezapomeňte na <a href="https://en.wikipedia.org/wiki/SOLID_(object-oriented_design)">SOLID</a> a zejména <a href="https://en.wikipedia.org/wiki/Single_responsibility_principle">Single Responsibility Principle</a>.</li>
<li>Některé soubory využívají vlastnosti jazyka C++, které zatím v předmětu nebyly vysvětleny. To ale nevadí :)</li>
<li>Vhodné použití visitorů vám může ulehčit implementaci. Nejste-li si jisti jejich vhodností pro zamýšlený účel, můžete se zeptat na cvičeních.</li>
<li>Používejte funkce z <code>Geometry.h</code>.</li>
<li>Pokud některé přiložené testy padají, znamená to, že jimi testovaná funkcionalita není korektně implementovaná.</li>
</ul>
<h1 id="přílohy">Přílohy</h1>
<h2 id="technické-záležitosti">Technické záležitosti</h2>
<h3 id="vývoj-bez-sdl">Vývoj bez SDL</h3>
<p>Úlohu je možné napsat i bez knihovny SDL2 – veškerá požadovaná funkcionalita je testovatelná testy v Catchi. Vypnout použití knihovny SDL2 lze tak, že předáte CMaku přepínač <code>-DUSE_SDL2=false</code>.</p>
<h3 id="vývoj-na-linuxu">Vývoj na Linuxu</h3>
<p>Bude se vám hodit knihovna SDL2. Fedora:</p>
<pre><code>sudo dnf install SDL2 SDL2-devel</code></pre>
<p>Ubuntu (i ve WSL):</p>
<pre><code>sudo apt install libsdl2-dev</code></pre>
<h3 id="softwarové-renderování">Softwarové renderování</h3>
<p>Pokud vám grafická hra po spuštění spadne s hláškou</p>
<pre><code>Cannot create renderer: Couldn't find matching render driver</code></pre>
<p>může pomoci změnit řádek 50 v <code>SDLWrapper.cpp</code> z</p>
<pre><code>: renderer_(SDL_CreateRenderer(&amp;window, -1, SDL_RENDERER_ACCELERATED)) {</code></pre>
<p>na</p>
<pre><code>: renderer_(SDL_CreateRenderer(&amp;window, -1, SDL_RENDERER_SOFTWARE)) {</code></pre>
<h3 id="vývoj-na-windows">Vývoj na Windows</h3>
<p>Matúš Talčík vytvořil <a href="https://is.muni.cz/auth/el/1433/podzim2018/PB161/um/hw/hw03/bomberman-cmake-windows.zip">šablonu</a> pro snadnou práci v prostředí Windows. Tato obsahuje upravený CMakeLists.txt a předkompilované knihovny pro použití MSVC a MinGW. Ve Visual studiu by měla fungovat bez dalších úprav; používáte-li však CLion, je třeba nakonfigurovat jej tak, aby jako toolchain používal MinGW.</p>
<p>MinGW můžete nainstalovat například přes <a href="https://sourceforge.net/projects/mingw-w64/files/latest/download?source=files">mingw-builds</a>; při instalaci (na <a href="https://imgur.com/HZtiq80">této obrazovce</a>) vyberte použití Posixových vláken.</p>
<h4 id="windows-subsystem-for-linux">Windows subsystem for Linux</h4>
<p>Úlohu je možné vypracovat i v prostředí Windows pomocí Windows Subsystem for Linux. Je ale třeba nainstalovat a spustit <a href="https://en.wikipedia.org/wiki/Xming">XServer</a> do Windows, nainstalovat Ubuntu z Microsoft Store, do Ubuntu nainstalovat závislosti (<code>libsdl2-dev</code>) a začít používat onen X server:</p>
<pre><code>export DISPLAY=:0</code></pre>
<h3 id="referenční-implementace">Referenční implementace</h3>
<h4 id="na-školních-linuxových-počítačích">Na školních linuxových počítačích</h4>
<p>Vzorová implementace je spustitelná na strojích <code>nymfeXY</code> (linuxové stroje v PC hale a B130) v <code>/home/xtusil/bomberman</code>.</p>
<h4 id="balíček-se-vzorovým-řešením-pro-linux">Balíček se vzorovým řešením pro Linux</h4>
<p><a href="https://is.muni.cz/auth/el/1433/podzim2018/PB161/um/hw/hw03/bomberman-linux-demo.zip">Stáhnout</a>, rozbalit a spustit pomocí</p>
<pre><code>env LD_LIBRARY_PATH=&quot;.&quot; ./bomberman</code></pre>
<h4 id="balíček-se-vzorovým-řešením-pro-windows">Balíček se vzorovým řešením pro Windows</h4>
<p>Zatím není</p>
<h3 id="přepínače-příkazové-řádky">Přepínače příkazové řádky</h3>
<p>Hra podporuje parametr <code>--celltime</code>, jímž je možné zpomalit běh celé hry. Hodnotou je čas v milisekundách, jak dlouho hráči trvá, než přejde z jednoho políčka do druhého. Defaultní hodnota je 250, hráč tedy dokáže ujít čtyři políčka za sekundu. Hru si tedy můžete spustit zpomaleně nebo zrychleně podle svého uvážení.</p>
<pre><code>/home/xtusil/bomberman --celltime 1000</code></pre>
<p>Mapu lze specifikovat pomocí přepínače <code>--map</code>.</p>
<h2 id="visitor-pattern">Visitor Pattern</h2>
<p>Uvažme následující situaci, která může nastat ve hře podobné té naší. Když exploduje bomba, s herními objekty v dosahu exploze se něco stane. Co přesně, to závisí na typu objektu: kamenná stěna výbuchu odolá, cihlová se zničí, bonusový objekt je zničen. Jedna z možností řešení této situace spočívá v přidání nové metody do rozhraní <code>IGameObject</code></p>
<pre><code>virtual void onBombExplosion() = 0;</code></pre>
<p>a v její implementaci veškerými herními objekty. Tento přístup má však několik nevýhod:</p>
<ol type="1">
<li>Stejně bychom moli přidat metody jako <code>whenPlayerBumpsIntoThis(Player &amp;)</code> nebo <code>onBulletHit()</code>. Do rozhraní se tedy dostávají metody úzce svázané s jedním konkrétním použitím, a jejich názvy a typy parametrů prozrazují něco, co by nemusely.</li>
<li>Implementace výbuchu bomby je rozsypaná na několika místech zdrojového kódu.</li>
</ol>
<h3 id="jednoduchý-visitor">Jednoduchý visitor</h3>
<p>Oproti tomu, při použití visitor patternu je veškerý kód pro výbuch bomby umístěný na jednom místě - v samostatné třídě, která pro každý typ herního objektu obsahuje samostatnou metodu.</p>
<pre><code>struct ExplosionVisitor {
    void onWall(Wall &amp; w) { if (w.isDestroyable()) { w.destroy(); } }
    void onPlayer(Player &amp; p) { p.kill(); }
    void onBomb(Bomb &amp; b) { b.explode(); }
};</code></pre>
<p>Instanci této třídy pak předáme hernímu objektu (neznámého typu), který je v dosahu výbuchu bomby.</p>
<pre><code>IGameObject &amp; object = /*...*/
ExplosionVisitor visitor;
object.visit(visitor);</code></pre>
<p>Herní objekt pak vybere, jakou funkci na našem visitoru zavolá.</p>
<pre><code>void Wall::visit(IVisitor &amp;visitor) {
    visitor.onWall(*this);
}</code></pre>
<p>Aby toto mohlo fungovat, v rozhraní <code>IGameObject</code> se musí nacházet funkce, které je možné <code>ExplosionVisitor</code> předat. Toho docílíme vytvořením rozhraní <code>IVisitor</code> a jeho implementací třídou <code>ExplosionVisitor</code>.</p>
<pre><code>struct IVisitor {
    virtual void onWall(Wall &amp; w) = 0;
    virtual void onPlayer(Player &amp; p) = 0;
    virtual void onBomb(Bomb &amp; b) = 0;
};
struct IGameObject {
    virtual void visit(IVisitor &amp; visitor) = 0;
};
struct ExplosionVisitor : IVisitor {
    /* ... */
};</code></pre>
<p>Toto už je funkční řešení našeho problému.</p>
<h3 id="hezčí-visitor">Hezčí visitor</h3>
<p>Visitory můžeme ještě trochu zkrášlit. Nejprve si všimněme, že metody ve visitoru můžeme pojmenovat všechny stejně.</p>
<pre><code>struct IVisitor {
    virtual void onObject(Wall &amp; w) = 0;
    virtual void onObject(Player &amp; p) = 0;
    virtual void onObject(Bomb &amp; b) = 0;
};

void Wall::visit(IVisitor &amp;visitor) {
    visitor.onObject(*this);
}</code></pre>
<p>Metoda <code>onObject</code> je tu přetížená pro každý typ herního objektu, a při jejím zavolání dojde v čase překladu k výběru správné verze. Další vylepšení je, že místo metody <code>onObject</code> přetížíme <code>operator()</code>:</p>
<pre><code>struct IVisitor {
    virtual void operator()(Bomb &amp; b) = 0;
    /* ... */
};

void Wall::visit(IVisitor &amp;visitor) {
    visitor(*this);
}</code></pre>
<p>Volání visitoru pak vypadá stejně jako volání obyčejné funkce. Takto vypadají i visitory v naší domácí úloze.</p>
  </div>
