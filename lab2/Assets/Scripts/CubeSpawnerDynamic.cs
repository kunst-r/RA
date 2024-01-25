using System.Collections.Generic;
using UnityEngine;

public class CubeSpawnerDynamic : MonoBehaviour
{
    public Queue<GameObject> QueuedCubes;
    public GameObject Origin;
    public GameObject CubePrefab;
    public float SpawnRate;
    public int MaxCubes;

    // Start is called before the first frame update
    private void Start()
    {
        QueuedCubes = new Queue<GameObject>();

        InvokeRepeating("SpawnCubes", 0, 1 / SpawnRate);
    }

    private void SpawnCubes()
    {
        GameObject CurrentCube = Instantiate(CubePrefab, Origin.transform.position, Quaternion.identity);
        Vector3 RandomForce = new Vector3(Random.Range(-100, 100), 500f, Random.Range(-100, 100));
        CurrentCube.GetComponent<Rigidbody>().AddForce(RandomForce);
        QueuedCubes.Enqueue(CurrentCube);
    }
}
