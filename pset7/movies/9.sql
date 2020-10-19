/* List the names of all people who starred in a movie
 released in 2004, ordered by birth year.
 If a person appeared in more than one movie in 2004,
 they should only appear in your results once.*/
SELECT
  name
FROM
  people
  JOIN stars ON stars.person_id = people.id
  JOIN movies ON stars.movie_id = movies.id
WHERE
  year = 2004
GROUP BY
  people.id
ORDER BY
  birth;