# Client config and contents files

Files the 2.20 client reads from its install folder. All are written by hand (or shipped by the
patcher); of the configuration/content files the client writes only `contents\contents1000.txt`
(it also writes screenshots as BMP, `DXC_ddraw.cpp:752-757`, and the `.tmp` decode files of the
badword section, `Misc.cpp:517`). Tags: [V] read in code, [I] inferred.
`C` = `sources/client-2.20/`; samples in `downloads/retail_2001/extracted/` are the **2001 retail
(V2.0)** data, older than the 2.20 client, so sample and code can disagree.

## Contents
- LOGIN.CFG
- contents\ItemName.cfg
- contents\contents<N>.txt (shop lists and text dialogs)
- contents\magiccfg.txt, Skillcfg.txt
- contents\BItemcfg.txt
- contents\GameMsgList.txt
- contents\badword.txt and Curse.txt (XOR-obfuscated)
- Gaps

## LOGIN.CFG

`CGame::bReadLoginConfigFile` `C/Game.cpp:15526`, opened as `login.cfg` (`:882`) [V]. Keys
`log-server-address` (18) and `log-server-port` (15) (`:15607-15608`). Before reading, the
function assigns hard-coded log addresses per build (`:15530-15549`) [V]. Sample:
`[CONFIG]`, `log-server-address = 211.239.170.90`, `log-server-port = 2848`.

## contents\ItemName.cfg

`bReadItemNameConfigFile` `C/Game.cpp:24707`, path `:24720` [V]. Record `Item = <origin name>
<display name>`, separators include `=` so runs of `=` act as padding (sample
`Item=(S.C)=====Dagger(S.C)=`) [V code `:24739-24754`; padding effect I]. Maps the server item
name (`m_cOriginName`) to the localised display name (`m_cName`).

## contents\contents<N>.txt

Two different uses of the same file name pattern [V]:

- **Shop list**: `_LoadShopMenuContents(cType)` `C/Game.cpp:7939` loads `contents<cType>.txt` and
  parses `ItemForSale`-records (keyword tested with 4 bytes, so any `Item...` token matches,
  `:8100`) via `__bDecodeContentsAndBuildItemForSaleList` `:7973`. Fields
  (`:7995-8086`): Name, Type, EPos, (Effect, skipped), ETValue1..6, MaxLife, (MFixC, skipped),
  Sprite, SFrame, Price, Wgt, (ApprV, skipped), Speed, LevelLimit. Matches the comment header in
  sample `contents1.txt`.
- **Text dialog**: `_LoadTextDlgContents(cType)` `:18871` and `_iLoadTextDlgContents2(type)`
  `:18929` (called with `sV1+20`, `:18316`) read the whole file, one dialog line per `\n`
  (sample `contents10.txt` is the about/credits text). `_LoadAgreementTextContents` `:26457` does
  the same for the agreement text.
- **contents1000.txt** is **written** by the client: `NoticementHandler` (`C/Game.cpp:24585-24603`)
  saves the notice text from `MSGID_RESPONSE_NOTICEMENT` with type REJECT (= new notice) [V].

## contents\magiccfg.txt, Skillcfg.txt

- `bInitMagicCfgList` `C/Game.cpp:8326` (`:8342`): `magic = Num Name ManaCost ReqInt Cost x x x
  OnMenu` - reads Num, Name, Value1..3, skips three, then OnMenu as `m_bIsVisible`
  (`:8377-8469`) [V].
- `bInitSkillCfgList` `:8521` (`:8537`): `skill = Num Name IsUseable UseMethod` (`:8571-8608`) [V].

These are the client's display copies; the authoritative Magic.cfg / Skill.cfg are server-side
(`server-content-cfg.md`).

## contents\BItemcfg.txt

`_bDecodeBuildItemContents` `C/Game.cpp:23978` -> `__bDecodeBuildItemContents` `:24271` [V].
`BuildItem = Name SkillLimit (ElementName Count) x6 SprH SprFrame MaxSkill`, `xxx 0` for an
empty element (`:24294-24380`). Differs from the server BuildItem.cfg (names, not item IDs).

## contents\GameMsgList.txt

`_LoadGameMsgTextContents` `C/Game.cpp:18990`, separators `";\n"`: each `;`-terminated string is
one entry of `m_pGameMsgList`, addressed by index [V].

## contents\badword.txt and Curse.txt (XOR-obfuscated)

`C/Game.cpp:994-1010` [V]: each file is decoded by `CMisc::_iConvertFileXor` (`C/Misc.cpp:478`) to
a `.tmp`, read, then deleted. Encoded layout: 10-byte header = decimal length of the body as
ASCII, each byte XOR 20, NUL-terminated; then the body, each byte XOR 35 (key passed at
`Game.cpp:995/1004`). The decode fails if the header length differs from file size minus 10 [V
`Misc.cpp:486-534`].

- badword: `CGameMonitor::iReadBadWordFileList` `C/GameMonitor.cpp:27`, tokens split on
  `"/,\t\n"`, up to `DEF_MAXBADWORD` words [V].
- Curse: `CCurse::LoadCurse` `C/Curse.cpp:36`, line based, lines starting `$`, `;` or newline
  skipped, the rest concatenated into one buffer (`MAX_CURSE_STRING`) [V].
- A second, plain-text badword reader exists at `C/Game.cpp:8503` [V; whether it is reached not
  traced].

## Gaps

- The 2001 retail data has no `Curse.txt`; its `badword.txt` header bytes do not look like the
  XOR-20 decimal header [I from a hex dump], so it predates the 2.20 obfuscation.
- `contents<N>.txt` number assignments (which N is which shop or dialog) are set by the server's
  dialog messages and are not catalogued here.
- `URL.txt` reader is commented out (`C/Game.cpp:169`).
