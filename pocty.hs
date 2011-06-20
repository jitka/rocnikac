import Data.List
import Data.Ord
import Data.Function

vrcholu = 18

-- pomocny binec..
nad :: (Integral a) => a -> a -> a
nad a 0 = 1
nad a b | a >= b = (a * ((a-1) `nad` (b-1))) `div` b

jePrvocislo x = and $ ( (x>1) : ) $ map (/=0) $ map ( x `mod` ) $ takeWhile ( \a -> a*a <= x ) [2..]

prvocisla = filter jePrvocislo [2..]

-- rozklad :: Integereger -> [Integereger]
rozklad = rozklad' prvocisla
        where 
--        rozklad' :: [Integereger] -> Integereger -> [Integereger]
        rozklad' _ 1 = []
        rozklad' (a:zbytek) n | n `mod` a == 0 = a : rozklad' (a:zbytek) (n `div` a)
                              | otherwise = rozklad' zbytek n

cifernyRozkladBy z = reverse . unfoldr (\x -> if x>0 then Just(x`mod`z,x`div`z) else Nothing)
cifernyRozklad = cifernyRozkladBy 10

scvrkavani :: (Eq a) => [a] -> [(a,Integer)] --setrizenych
scvrkavani = map (\a -> ( head a, genericLength a )) . group 

scvrkavani2 = map (foldl1 (\(z,p1) (_,p2) -> (z,p1+p2))) . groupBy ((==)`on`fst)

l = length . cifernyRozklad 

nsd a b | b == 0 = a
        | otherwise = nsd b (a `mod` b)

nsn a b = (a*b) `div` (nsd a b)

--generuje vsechny setrizene zpusoby jak nastitat k
rozdel n = rozdel' 1 n
        where
        rozdel' _ 0 = [[]]
        rozdel' k n | k > n = []
                    | otherwise = map (k:) (rozdel' k (n-k)) ++ rozdel' (k+1) n


-- !!!!TU TO ZACINA!!!!
-- budu se snazit zjistit neco o tom kolik ma hra stavu
-- pokracujici hra je takova, kdyz se nezastavi pri vyteztvi nekoho a pokracuje se dal
-- zacina zeleny hrac a pak hraje cerveny

-- pocet stavu pokracujici hry po k-tem tahu
f1 n tah = let pul = ((tah+1) `div` 2)
               hran = n `nad` 2 
           in (hran `nad` pul) * ( (hran-pul) `nad` (tah-pul) )
--je:
f2 = map (f1 vrcholu) [0..(vrcholu `nad` 2)]
--a to nejvice po tahu:
f3 = maximumBy (comparing (f1 vrcholu)) [0..(vrcholu `nad` 2)]

--celkove je stavu pokracujici hry, logaritmus:
f4 = (sum f2, l $ sum f2)

--
--zpocitam stavy pokracujici hry az na izomorfizmus z Burnsideova lemmatu
--

--kolik je fixpointu vuci permutaci=ciklu na k vrcholech:
--pokud je v grafu jedna hrana ktera jde pres krok vrcholu v cyklu pak tam 
--musi byt i spousta dalsich, vyberu  a nebo nevyberu jim barvu a zkusim dalsi..
{-k1 k krok zel cer | zel < 0 || cer < 0 = 0 --jak bych barvila -3 hrany?
                  | zel == 0 && cer == 0 = 1 --hotovo a spravne
                  | krok*2 == k && zel == krok && cer == 0 = 1 --protejsi hrany obarvim zelene a vse je hotovo spravne
                  | krok*2 == k && cer == krok && zel == 0 = 1 --protejsi hrany obarvim cervene a vse je hotovo spravne
                  | krok*2 == k = 0 --v tomto stavu uz nejde doobarvit tak aby vysel fixpoint vuci tomu ciklu
                  | krok*2 > k = 0 --hotovo a spatne
                  | otherwise = (k1 k (krok+1) zel cer) +
                                (k1 k (krok+1) (zel-k) cer) +
                                (k1 k (krok+1) zel (cer-k) )
-}
--pri pouziti nanejvys zel a cer hran. Vrcaci jako seznam zbytku zel a cer
k1' k krok (zel,cer) | zel < 0 || cer < 0 = [] --jak bych barvila -3 hrany?
                     | krok*2 == k = (k1' k (krok+1) (zel,cer)) ++
                                     (k1' k (krok+1) (zel-krok,cer)) ++
                                     (k1' k (krok+1) (zel,cer-krok))
                     | krok*2 >= k = [(zel,cer)] --projite vsechny rozmisteni vrcholu
                     | otherwise = (k1' k (krok+1) (zel,cer)) ++
                                   (k1' k (krok+1) (zel-k,cer)) ++
                                   (k1' k (krok+1) (zel,cer-k))
k1 :: Integer -> (Integer,Integer) -> [((Integer,Integer),Integer)]
k1 k (zel,cer) = scvrkavani $ sort $ k1' k 1 (zel,cer)

--jak muze dopadnout obarvovani skupinek hran


{--tato implementace je exponencialni
k2' :: [Integer] -> (Integer,Integer) -> [(Integer,Integer)]
k2' [] (zel,cer) | zel < 0 || cer < 0  = []
                 | otherwise = [(zel,cer)]
k2' (s:skupinky) (zel,cer) | zel < 0 || cer < 0 = [] --neda se obarvit zaporny pocet hran
                           | otherwise = (k2' skupinky (zel,cer)) ++
                                         (k2' skupinky (zel-s,cer)) ++
                                         (k2' skupinky (zel,cer-s))
k2 skupinky barvy = scvrkavani $ sort $ k2' skupinky barvy

--kolik je fixpointu vuci permutaci=pole velikosti cyklu
k3' :: [Integer] -> [((Integer,Integer),Integer)] -> [((Integer,Integer),Integer)]
k3' [] zbytky = zbytky
k3' (vel:permutace) zbytky = 
        let pridej_vel (barvy,nasobek) = map (\(b,p)->(b,p*nasobek)) $ k1 vel barvy
            obarveni1 = scvrkavani2 $ sort $ concat $ map pridej_vel zbytky
            --skupiny hran mezi prvnim a dalsimy cykly, ktere musi mit stejnou barvu
            hrany = sort $ concat $ map (\x -> replicate (fromIntegral $ nsd vel x) (nsn vel x)) permutace 
            pridej_skup (barvy,nasobek) = map (\(b,p)->(b,p*nasobek)) $ k2 hrany barvy
            obarveni2 = scvrkavani2 $ (((0,0),0) :) $ sort $ concat $ map pridej_skup obarveni1
        in k3' permutace obarveni2
k3 tah permutace = 
        let pul = ((tah+1) `div` 2)
        in  snd $ head $ k3' permutace [((pul,tah-pul),1)]

--a konecne pocitani vsech moznych stavu hry az na izomorfizmus po nejakem tahu
f5 tah = map (\perm -> k3 tah perm) $ rozdel vrcholu
-}




