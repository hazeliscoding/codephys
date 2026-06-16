# References

CodePhys implements introductory physics and standard numerical methods. This
page lists the **primary literature** behind each piece, so every integrator,
law, and constant can be traced to its source — not only to the course textbook.
Machine-readable BibTeX for all of these is in [`../REFERENCES.bib`](../REFERENCES.bib).

> **Verification.** Every entry with a DOI was checked against
> [CrossRef](https://api.crossref.org) (title, authors, volume, pages, year).
> Pre-DOI classics (Newton, Kepler, Euler, Kutta, Störmer) are formatted from
> their canonical citations. The textbook anchoring the study guide is OpenStax
> *College Physics 2e* ([Urone & Hinrichs 2022](#openstax)).

---

## Which source backs which part of the code

| Code | Concept | Primary source(s) |
|---|---|---|
| `physics/src/integrator.cpp` — Explicit Euler | forward Euler step | [Euler 1768](#euler1768) |
| `physics/src/integrator.cpp` — Semi-implicit Euler | symplectic / Störmer step | [Störmer 1907](#stormer1907); [Hairer, Lubich & Wanner 2003](#hlw2003) |
| `physics/src/integrator.cpp` — Velocity Verlet | velocity form of Verlet | [Verlet 1967](#verlet1967); [Swope et al. 1982](#swope1982) |
| `physics/src/integrator.cpp` — RK4 | classical 4th-order Runge–Kutta | [Runge 1895](#runge1895); [Kutta 1901](#kutta1901) |
| *integrator theory / orders / symplecticity* | geometric integration | [Hairer, Nørsett & Wanner 1993](#hnw1993); [Hairer, Lubich & Wanner 2006](#hlw2006) |
| `physics/src/world.cpp`, `app/scenes/ch03_projectile.cpp` | Newton's laws, kinematics, gravity | [Newton 1687](#newton1687) |
| `app/scenes/integrator_comparison.cpp`, orbit test | circular orbits, Kepler's third law | [Kepler 1609](#kepler1609); [Kepler 1619](#kepler1619) |
| `world.cpp` energy/momentum/angular-momentum readouts; `tests/test_physics.cpp` | conservation laws | [Noether 1918](#noether1918); [Goldstein et al. 2002](#goldstein2002) |
| `physics/include/physics/math/constants.hpp` | g, G, c, h, k_B, ε₀, … | [Tiesinga et al. 2021 (CODATA 2018)](#codata2018); [BIPM SI 2019](#bipm2019) |
| study guide | course textbook | [OpenStax *College Physics 2e* 2022](#openstax) |

---

## Full list

### Numerical integration of ODEs
- <a id="euler1768"></a>**Euler, L.** (1768). *Institutionum calculi integralis*, Vol. 1. Imperial Academy of Sciences, St. Petersburg. — origin of the forward (explicit) Euler method.
- <a id="runge1895"></a>**Runge, C.** (1895). Über die numerische Auflösung von Differentialgleichungen. *Mathematische Annalen* **46**(2), 167–178. DOI: [10.1007/BF01446807](https://doi.org/10.1007/BF01446807)
- <a id="kutta1901"></a>**Kutta, W.** (1901). Beitrag zur näherungsweisen Integration totaler Differentialgleichungen. *Zeitschrift für Mathematik und Physik* **46**, 435–453. — with Runge, the classical RK4 method.
- <a id="stormer1907"></a>**Störmer, C.** (1907). Sur les trajectoires des corpuscules électrisés dans l'espace. *Archives des Sciences Physiques et Naturelles* **24**. — origin of the Störmer (leapfrog / symplectic) method.
- <a id="verlet1967"></a>**Verlet, L.** (1967). Computer "experiments" on classical fluids. I. *Physical Review* **159**(1), 98–103. DOI: [10.1103/PhysRev.159.98](https://doi.org/10.1103/PhysRev.159.98)
- <a id="swope1982"></a>**Swope, W. C., Andersen, H. C., Berens, P. H. & Wilson, K. R.** (1982). A computer simulation method … *The Journal of Chemical Physics* **76**(1), 637–649. DOI: [10.1063/1.442716](https://doi.org/10.1063/1.442716) — introduces the **velocity Verlet** form.
- <a id="hlw2003"></a>**Hairer, E., Lubich, C. & Wanner, G.** (2003). Geometric numerical integration illustrated by the Störmer–Verlet method. *Acta Numerica* **12**, 399–450. DOI: [10.1017/S0962492902000144](https://doi.org/10.1017/S0962492902000144)
- <a id="hnw1993"></a>**Hairer, E., Nørsett, S. P. & Wanner, G.** (1993). *Solving Ordinary Differential Equations I: Nonstiff Problems* (2nd ed.). Springer. DOI: [10.1007/978-3-540-78862-1](https://doi.org/10.1007/978-3-540-78862-1)
- <a id="hlw2006"></a>**Hairer, E., Lubich, C. & Wanner, G.** (2006). *Geometric Numerical Integration* (2nd ed.). Springer. DOI: [10.1007/3-540-30666-8](https://doi.org/10.1007/3-540-30666-8) — symplectic integrators and energy behaviour.

### Classical mechanics
- <a id="newton1687"></a>**Newton, I.** (1687). *Philosophiæ Naturalis Principia Mathematica.* Royal Society, London. — the three laws of motion and universal gravitation.
- <a id="kepler1609"></a>**Kepler, J.** (1609). *Astronomia Nova.* Prague. — Kepler's first and second laws.
- <a id="kepler1619"></a>**Kepler, J.** (1619). *Harmonices Mundi.* Linz. — Kepler's third law, T² ∝ a³.
- <a id="noether1918"></a>**Noether, E.** (1918). Invariante Variationsprobleme. *Nachr. Ges. Wiss. Göttingen, Math.-Phys. Kl.*, 235–257. — symmetries ⇒ conservation laws.
- <a id="goldstein2002"></a>**Goldstein, H., Poole, C. P. & Safko, J. L.** (2002). *Classical Mechanics* (3rd ed.). Addison-Wesley.

### Physical constants
- <a id="codata2018"></a>**Tiesinga, E., Mohr, P. J., Newell, D. B. & Taylor, B. N.** (2021). CODATA recommended values of the fundamental physical constants: 2018. *Reviews of Modern Physics* **93**(2), 025010. DOI: [10.1103/RevModPhys.93.025010](https://doi.org/10.1103/RevModPhys.93.025010)
- <a id="bipm2019"></a>**BIPM** (2019). *The International System of Units (SI)* (9th ed.). — exact c, h, k_B; standard gravity g = 9.80665 m/s².

### Course textbook
- <a id="openstax"></a>**Urone, P. P. & Hinrichs, R.** (2022). *College Physics 2e.* OpenStax, Rice University, Houston. <https://openstax.org/details/books/college-physics-2e>

---

*The links in the table jump to the matching entry; the corresponding BibTeX keys
are in [`../REFERENCES.bib`](../REFERENCES.bib).*
