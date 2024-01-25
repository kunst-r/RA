using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class CameraController : MonoBehaviour
{

    public Transform teddyTransform;

    void LateUpdate()
    {
        if (teddyTransform.position.y > transform.position.y)
        {
            transform.position = new Vector3(transform.position.x, teddyTransform.position.y, transform.position.z);
        }
    }
}
