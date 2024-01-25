using Unity.VisualScripting;
using UnityEngine;

public class SpriteBillboard : MonoBehaviour
{
    private Vector3 CameraPosition;

    // Update is called once per frame
    void Update()
    {
        CameraPosition = Camera.main.transform.position;
        this.transform.LookAt(CameraPosition);
    }
}
