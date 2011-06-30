import Data.List
import Data.Ord
import Data.Function
import Data.Array

import Control.DeepSeq
import Control.Parallel
import Control.Parallel.Strategies

-- pomocny binec..
nad :: (Integral a) => a -> a -> a
nad a 0 = 1
nad a b | a >= b = (a * ((a-1) `nad` (b-1))) `div` b

cifernyRozkladBy z = reverse . unfoldr (\x -> if x>0 then Just(x`mod`z,x`div`z) else Nothing)
cifernyRozklad = cifernyRozkladBy 10
l = length . cifernyRozklad 

nsd a b | b == 0 = a
        | otherwise = nsd b (a `mod` b)

nsn a b = (a*b) `div` (nsd a b)

-- !!!!TU TO ZACINA!!!!
-- budu se snazit zjistit neco o tom kolik ma stavu hra
-- takova, kdyz se nezastavi pri vyteztvi nekoho a pokracuje se dal
-- hraje je se na K_n
-- zacina zeleny hrac a pak hraje cerveny

-- pocet stavu hry po k-tem tahu
f1 n tah = let pul = ((tah+1) `div` 2)
               hran = n `nad` 2 
           in (hran `nad` pul) * ( (hran-pul) `nad` (tah-pul) )

-- pocet stavu hry po k-tem tahu az na izomorfismus
--zpocitam z Burnsideova lemmatu
--
--klicove je spocitat kolik grafu je fixpointem vuci nejake permutaci
--
--permutaci reprezuntuji typem: seznamem velikosti cyklu
--
--kdyz chci zjistit pocet fixpointu rozdelim si graf do nekolika skupin
--jedna skupina jsou hrany, oba vrcholy lezi v konkretnim cyklu permutace
--jina skpina jsou hrany ktere vedou mezi dvema cykli v permutaci
--
--budu pocitat vic nez musim, ne jen kolik je fixpointu po nejakem tahu
--ale rovnou kolik je fixpointu takovych ze z hran je zelenych a c cervenych.
--budu si je ukladat jako dvojici (pouzitych barev, pocet fixpointu)
--abych nemela v seznamu dvojici pro stejne barvy dvakrat, pouzivam funkci
--svrkavani
--
--kdyz uz mam takovy vysledek pro nejake skupiny, pro jejich sjednoceni
--ziskam vysledek tak, ze vezmu vsechny kombinace moznych obarveni jedne
--a druhe pocty hran sectu, protoze sjednocuji a moznosti kolik takovych grafu je
--vynasobim protoze obarveni prvni i druhe skupiny jsou nezavysle (vis funkce
--nasobek)
--
--vysledek jedne skupiny zistam tak, ze si hrany ze skupiny rozdelim do jeste
--jeste mensich skupinek a to tak ze ve skupince jsou hrany, ktere musi mit
--stejnou barvu. kdyz takove rozdeleni mam tak moznosti obarveni tvorim postumne
--zasnu s jednim nula barvami. kdyz pridam skupinku vezmu vnechny moznosti jak
--se barvilo bez ni + obarvit skupinku zelene pro kteroukoliv moznost barveni bez ni
-- + obarvit skupinku cervene pro kteroukoliv moznost
--viz fuknce obarvi
--
--pro skupinu hran patrici jednomu cyklu pokud cyklus je lichy jsou 
--hrany po obvodu, hrany ob jedno, hrany ob dva atd.. celkove n/2 skupinek
--velikosti n
--pro cyklus sudy ciklus navic pripocteme polovicni skupinku 'hrany naproti'
--viz skupinky1 ve funkci fixpointy
--
--pro skupinu hran hran mezi dvema cykly velikosti a a b: je to (nsd a b)
--skpinek velikosti (nsn a b) celkem tedy a*b hran jak ma byt
--viz skupinky2 ve fuknci fixpointy (tam jsou udelne dohromady pro skpiny
--mezi prove probiranym cyklem a vsemi jeste neprobiranimy)
--
--

scvrkavani :: Integer -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)]
scvrkavani n x = filter (\x->snd x /= 0) $ assocs $  accumArray (+) 0 ((0,0),(n`nad`2,n`nad`2)) x

obarvi :: Integer -> [Integer] -> [((Integer,Integer),Integer)]
obarvi n skupinky = foldl (k2' n) [((0,0),1)] skupinky where
        --k2' :: Integer -> [((Integer,Integer),Integer)] -> Integer -> [((Integer,Integer),Integer)]
        k2' n puvodni s = scvrkavani n $ (map (\((z,c),p) -> ((z,c+s),p) ) puvodni) ++ (map (\((z,c),p) -> ((z+s,c),p) ) puvodni) ++ puvodni

nasobek :: Integer -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)]
nasobek n a b = scvrkavani n [ ((z1+z2,c1+c2),p1*p2) | ((z1,c1),p1) <- a, ((z2,c2),p2) <- b ]

fixpointy :: Integer -> [Integer] -> [((Integer,Integer),Integer)]
fixpointy _ [] = [((0,0),1)]
fixpointy n (vel:permutace) = 
        let skupinky1 | vel `mod` 2 == 1 = replicate (fromIntegral vel `div` 2) vel
                      | otherwise = (vel`div`2) : replicate (fromIntegral (vel-1) `div` 2) vel
            obarveni1 = obarvi n skupinky1
            skupinky2 = reverse $ sort $ concat $ map (\x -> replicate (fromIntegral $ nsd vel x) (nsn vel x)) permutace
            obarveni2 = obarvi n skupinky2
        in nasobek n (nasobek n obarveni1 obarveni2) $ fixpointy n permutace

--generuje vsechny setrizene zpusoby jak nastitat k
typyPermutaci n = map reverse $ rozdel' 1 n
        where
        rozdel' _ 0 = [[]]
        rozdel' k n | k > n = []
                    | otherwise = map (k:) (rozdel' k (n-k)) ++ rozdel' (k+1) n

--kolik permutaci toho typu je TODO toto je asi blbe
fac x = product [1..x]
pocetTakovych permutace = (fac $ sum permutace) `div` ((product permutace) * (product $ map fac $ map genericLength $ group permutace))

--pocet stavu hry po nejakem tahu az na izomorfizmus 
f2 n tah = (sum $ map (\per -> (k4 n tah per)*(pocetTakovych per)) $ typyPermutaci n) `div` (product [1..n]) where
        --kolik fixpointu ma hra v nejakem tahu pro nejakou permutaci 
        k4 n tah permutace = snd $ head $ (filter (\((z,c),_) -> ( z==((tah+1)`div`2) && c==(tah`div`2) )) $ fixpointy n permutace) ++ [((0,0),0)]

-- rychlejsi vypis vsecho
f6 n = let tabulka = (map (\p -> (p,fixpointy n p)) $ typyPermutaci n) `using` parList rdeepseq
           vytahni' permutace = snd $ head $ filter (\x->fst x == permutace) tabulka
           barvy tah = ((tah+1)`div`2, tah`div`2)
           vytahni tah permutace = snd $ head $ (filter (\(b,_) -> ( b == barvy tah )) $ vytahni' permutace) ++ [((undefined,undefined),0)]
           vysl n tah = (sum $ map (\per -> (vytahni tah per)*(pocetTakovych per)) $ typyPermutaci n) `div` (product [1..n])
        in tabulka `deepseq` map (\tah -> (tah,l $ f1 n tah,l $ vysl n tah )) [1..(n`nad`2)]

main = print $ map (\x -> (x,f6 x)) [8..18]

