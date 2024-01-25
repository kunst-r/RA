using System.Collections.Generic;
using UnityEngine;

public class GeneratorController : MonoBehaviour
{
    public GameObject platformPrefab;
    public Queue<GameObject> platformPrefabs;
    public int numberOfPlatforms = 100;
    public float levelWidth = 3f;
    public float minX = -3f;
    public float maxX = 3f;
    public float minY = 3.5f;
    public float maxY = 4.5f;
    private Vector3 spawnPosition;

    public GameObject crushablePlatformPrefab;
    public Queue<GameObject> crushablePlatformPrefabs;
    private int crushableSpawnPercent = 30;
    private Vector3 crushableSpawnPosition;

    public GameObject beePrefab;
    public Queue<GameObject> beePrefabs;
    private int beeSpawnPercent = 5;
    private Vector3 beeSpawnPosition;

    private GameObject lowestObject;

    private GameObject teddy;
    private TeddyController teddyController;
    private float teddyMaxY;

    // Start is called before the first frame update
    void Start()
    {
        teddy = GameObject.Find("teddy");
        teddyController = teddy.GetComponent<TeddyController>();

        platformPrefabs = new Queue<GameObject>();
        crushablePlatformPrefabs = new Queue<GameObject>();
        beePrefabs = new Queue<GameObject>();

        spawnPosition = new Vector3();
        crushableSpawnPosition = new Vector3();
        beeSpawnPosition = new Vector3();


        for (int i = 0; i < numberOfPlatforms; i++)
        {
            // spawn new platform
            spawnPosition.x = Random.Range(minX, maxX);
            spawnPosition.y += Random.Range(minY, maxY);
            GameObject newPlatform = Instantiate(platformPrefab, spawnPosition, Quaternion.identity);
            platformPrefabs.Enqueue(newPlatform);

            // chance to spawn a crushable platform
            if (Random.Range(0, 100) < crushableSpawnPercent)
            {
                crushableSpawnPosition = spawnPosition;
                crushableSpawnPosition.x = Random.Range(minX, maxX);
                crushableSpawnPosition.y += Random.Range(minY, maxY);
                GameObject newCrushablePlatform = Instantiate(crushablePlatformPrefab, crushableSpawnPosition, Quaternion.identity);
                crushablePlatformPrefabs.Enqueue(newCrushablePlatform);
            }

            // chance to spawn a bee
            if (Random.Range(0, 100) < beeSpawnPercent)
            {
                beeSpawnPosition = spawnPosition;
                beeSpawnPosition.x = Random.Range(minX, maxX);
                beeSpawnPosition.y += Random.Range(minY, maxY);
                GameObject newBee = Instantiate(beePrefab, beeSpawnPosition, Quaternion.identity);
                beePrefabs.Enqueue(newBee);
            }
        }
    }

    private void Update()
    {
        teddyMaxY = teddyController.GetMaxY();

        RepositionPlatform();
        RepositionCrushablePlatform();
        RepositionBee();

        /*
        while (true)
        {
            // if no platform is repositioned, we probably didn't make any height progress
            if (!RepositionPlatform())
            {
                break;
            }
            RepositionCrushablePlatform();
            RepositionBee();

            
        }*/
    }

    private bool RepositionPlatform()
    {
        // reposition platforms that are too low (unreachable) to the top
        if (platformPrefabs.Count > 0) 
        { 
            lowestObject = platformPrefabs.Peek(); 
        } else
        {
            return false;
        }
        
        if (lowestObject.transform.position.y < teddyMaxY - 5.0f)
        {
            lowestObject = platformPrefabs.Dequeue();
            spawnPosition.x = Random.Range(minX, maxX);
            spawnPosition.y += Random.Range(minY, maxY);
            lowestObject.transform.position = spawnPosition;
            platformPrefabs.Enqueue(lowestObject);
            return true;
        }

        return false;
    }

    private bool RepositionCrushablePlatform()
    {
        // chance to reposition lowest crushable platform (if lower than screen)
        // also set it active (bc they could have been set inactive on collision)
        if (crushablePlatformPrefabs.Count > 0)
        {
            lowestObject = crushablePlatformPrefabs.Peek();
        } else
        {
            return false;
        }

        if (lowestObject.transform.position.y < teddyMaxY - 5.0f)
        {
            lowestObject = crushablePlatformPrefabs.Dequeue();
            crushableSpawnPosition = spawnPosition;
            crushableSpawnPosition.x = Random.Range(minX, maxX);
            crushableSpawnPosition.y = teddyMaxY + Random.Range(10 * minY, 15 * maxY);
            lowestObject.transform.position = crushableSpawnPosition;
            lowestObject.SetActive(true);
            crushablePlatformPrefabs.Enqueue(lowestObject);
        }

        return false;
        
    }

    private bool RepositionBee()
    {
        // chance to reposition lowest bee (if lower than screen)
        if (beePrefabs.Count > 0)
        {
            lowestObject = beePrefabs.Peek();
        } else
        {
            return false;
        }

        if (lowestObject.transform.position.y < teddyMaxY - 5.0f)
        {
            lowestObject = beePrefabs.Dequeue();
            beeSpawnPosition = spawnPosition;
            beeSpawnPosition.x = Random.Range(minX, maxX);
            beeSpawnPosition.y = teddyMaxY + Random.Range(10 * minY, 15 * maxY);
            lowestObject.transform.position = beeSpawnPosition;
            beePrefabs.Enqueue(lowestObject);
            return true;
        }

        return false;
        
    }
}
